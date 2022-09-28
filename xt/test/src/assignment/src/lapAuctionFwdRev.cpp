#include "../include/lapAuctionFwdRev.h"
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xsort.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    static bool isempty(const xt::xarray<double> &arr);
    static void auction(xt::xarray<double> &costMatrix,
                        double epsilon,
                        xt::xarray<double> &rowSoln,
                        xt::xarray<double> &colSoln,
                        xt::xarray<double> &rowRedux,
                        xt::xarray<double> &colRedux);
    static void computeBid(xt::xarray<double> &costs,
                           xt::xarray<bool> &isUnassigned,
                           double &bid,
                           size_t &bestInd);

    void lapAuctionFwdRev(xt::xarray<double> &costMatrix,
                          xt::xarray<double> &rowSoln,
                          xt::xarray<double> &colSoln,
                          xt::xarray<double> &colRedux,
                          double maxAuctions,
                          bool hasEpsilonScaling)
    {
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        double nan = std::numeric_limits<double>::quiet_NaN();
        if (isempty(rowSoln))
        {
            rowSoln = xt::xarray<double>({nRow}, nan);
        }
        if (isempty(colSoln))
        {
            colSoln = xt::xarray<double>({nCol}, nan);
        }
        if (isempty(colRedux))
        {
            colRedux = xt::zeros<double>({nCol});
        }

        // Nothing to do?
        if (!xt::any(xt::isnan(rowSoln)))
        {
            return;
        }

        // Bertsekas' recommendations for e-scaling:
        //
        //   bidFactor in the bounds [2 5]
        //   epsFactor in the bounds [4, 10] (forward-reverse auction should use
        //                                    larger values)
        //   epsilon in the bounds [minBid, maxBid] (note that setting to
        //                                           minBid disables e-scaling)
        //
        // Auction parameters below selected to give best performance across broad
        // range of assignment problems

        double bidFactor = 4;
        double epsFactor = 50;
        xt::xarray<double> v = xt::filter(costMatrix, xt::isfinite(costMatrix));
        double maxRes = xt::nanmax(v)() - xt::nanmin(v)();
        double minRes = xt::nanmin(xt::diff(xt::unique(v)))();
        double maxBid = maxRes / bidFactor / (nRow + 1); // [ maxRes / 5, maxRes / 2 ]
        double minBid = minRes / (nRow + 1);             // Set to satisfy the optimality condition
        double epsilon = hasEpsilonScaling ? maxBid : minBid;

        // Loop over epsilon scaling phases
        // Each phase improves the estimate of the column reduction values, which
        // leads to faster convergence for the next auction phase using smaller bid
        // values. Optimal assignment is guaranteed when epsilon < 1/n

        // Init row reduction values for reverse auction
        xt::xarray<double> rowRedux = xt::zeros<double>({nRow});

        xt::xarray<double> costMatrix_t = xt::transpose(costMatrix);
        bool lastPhase = false;
        int auctionCount;
        while (!lastPhase)
        {
            // Reset assignment to begin next scaling phase
            auctionCount = 0;
            rowSoln = xt::xarray<double>({nRow}, nan);
            colSoln = xt::xarray<double>({nCol}, nan);

            lastPhase = epsilon <= minBid;

            // Perform auction until all rows are assigned or max iteration count is reached
            while (xt::any(xt::isnan(rowSoln)) && !(lastPhase && auctionCount >= maxAuctions))
            {
                // Forward auction
                auction(costMatrix, epsilon, rowSoln, colSoln, rowRedux, colRedux);
                auctionCount = auctionCount + 1;

                if (!xt::any(xt::isnan(rowSoln)) || (lastPhase && auctionCount >= maxAuctions))
                {
                    break;
                }

                // Reverse auction
                auction(costMatrix_t, epsilon, colSoln, rowSoln, colRedux, rowRedux);
                auctionCount = auctionCount + 1;
            }
            if (epsilon <= minBid)
            {
                break;
            }
            // Update scaling
            epsilon = std::max(epsilon / epsFactor, minBid);
        }
    }

    static bool isempty(const xt::xarray<double> &arr)
    {
        return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
    }

    static void auction(xt::xarray<double> &costMatrix,
                        double epsilon,
                        xt::xarray<double> &rowSoln,
                        xt::xarray<double> &colSoln,
                        xt::xarray<double> &rowRedux,
                        xt::xarray<double> &colRedux)
    {
        // Performs the forward Auction linear assignment algorithm until the number
        // of free rows is reduced by at least one

        xt::xarray<bool> isUnassigned = xt::isnan(rowSoln);

        // Nothing to be done?
        if (!xt::any(isUnassigned))
        {
            return;
        }

        size_t nAssigned = xt::sum(!isUnassigned)();

        // This auction phase may get stuck in "price wars" when epsilon is low.
        // Start checking for price war iterations when number of iterations are
        // high.
        size_t numIterations = 1;
        size_t priceWarIter = 1;
        bool initPriceWarCheck = false;

        xt::xarray<bool> isNaNcolSoln;
        xt::xarray<size_t> rowList;
        xt::xarray<double> initPrices, costs, finalPrices;
        size_t bestCol;
        double bid, prevRow;
        while ((xt::sum(!isUnassigned)() < (nAssigned + 1)) && (priceWarIter < 1000u))
        {
            numIterations = numIterations + 1;
            initPrices = colRedux;
            rowList = xt::flatten_indices(xt::argwhere(isUnassigned));
            for (size_t freeRow : rowList) // Find the next row to assign
            {
                //  Bidding phase: find the best and 2nd best column
                costs = xt::row(costMatrix, freeRow) - colRedux;

                isNaNcolSoln = xt::isnan(colSoln);
                computeBid(costs, isNaNcolSoln, bid, bestCol);
                bid = bid + epsilon;
                colRedux.at(bestCol) = colRedux.at(bestCol) - bid;
                rowRedux.at(freeRow) = costMatrix.at(freeRow, bestCol) - colRedux.at(bestCol);

                // Assign this row to the min - valued column
                prevRow = colSoln(bestCol);
                colSoln.at(bestCol) = freeRow;
                rowSoln.at(freeRow) = bestCol;

                // If the minimum column was previously assigned to a row, then
                // unassign it
                if (!std::isnan(prevRow))
                {
                    rowSoln.at(prevRow) = std::numeric_limits<double>::quiet_NaN();
                }
            }
            finalPrices = colRedux;
            // Price wars - Protracted sequences of small price rises (or profit
            // reduction) resulting from group of objects competing for a smaller
            // number of roughly equal desirable objects [1].
            if (initPriceWarCheck || numIterations > 100000)
            {
                initPriceWarCheck = true;
                if (xt::all(xt::abs(finalPrices - initPrices) <= epsilon + 1.73472347597681e-18))
                {
                    priceWarIter = priceWarIter + 1;
                }
                else
                {
                    // No price war in last bidding phase.
                    priceWarIter = 1;
                }
            }
            // Check the number of unassigned rows
            isUnassigned = xt::isnan(rowSoln);
        }
    }

    static void computeBid(xt::xarray<double> &costs,
                           xt::xarray<bool> &isUnassigned,
                           double &bid,
                           size_t &bestInd)
    {
        // Returns the 2 smallest values and their corresponding indices in costs.
        // When multiple values at the same minimum are found, will return a value
        // corresponding to an unassigned index as the first minimum
        double min1 = xt::nanmin(costs)(), min2;
        xt::xarray<size_t> minInds = xt::flatten_indices(xt::argwhere(xt::equal(costs, min1)));
        bestInd = minInds.at(0);
        xt::xarray<bool> isMinUnassigned;
        if (minInds.size() > 1)
        {
            minInds = xt::view(minInds, xt::range(1, _));
            isMinUnassigned = xt::index_view(isUnassigned, minInds);
            if (xt::any(isMinUnassigned))
            {
                minInds = xt::filter(minInds, isMinUnassigned);
                bestInd = minInds.at(0);
            }
            bid = 0.;
        }
        else
        {
            // Find second smallest minimum
            xt::index_view(costs, minInds) = std::numeric_limits<double>::quiet_NaN();
            min2 = xt::nanmin(costs)();
            bid = min2 - min1;
        }
    }
}