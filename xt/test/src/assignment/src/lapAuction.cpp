#include "../include/lapAuction.h"
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xsort.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    static bool isempty(const xt::xarray<double> &arr);
    static void auction(const xt::xarray<double> &costMatrix,
                        double epsilon,
                        xt::xarray<double> &rowSoln,
                        xt::xarray<double> &colSoln,
                        xt::xarray<double> &colRedux);
    static void computeBid(xt::xarray<double> &costs,
                           xt::xarray<bool> &isUnassigned,
                           double &bid,
                           size_t &bestInd);

    void lapAuction(const xt::xarray<double> &costMatrix,
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
        double epsFactor = 7;
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
                auction(costMatrix, epsilon, rowSoln, colSoln, colRedux);
                auctionCount = auctionCount + 1;
            }

            // Update scaling
            epsilon = std::max(epsilon / epsFactor, minBid);
        }
    }

    static bool isempty(const xt::xarray<double> &arr)
    {
        return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
    }

    static void auction(const xt::xarray<double> &costMatrix,
                        double epsilon,
                        xt::xarray<double> &rowSoln,
                        xt::xarray<double> &colSoln,
                        xt::xarray<double> &colRedux)
    {
        // Performs one iteration of the forward Auction linear assignment algorithm

        // Each Auction iteration cycles over all free rows
        xt::xarray<size_t> rowList = xt::flatten_indices(xt::argwhere(xt::isnan(rowSoln)));
        xt::xarray<double> costs;
        xt::xarray<bool> isUnassigned;
        size_t bestCol;
        double bid, rowPrev;
        double nan = std::numeric_limits<double>::quiet_NaN();
        for (size_t rowFree : rowList) // Find the next row to assign
        {
            //  Bidding phase: find the best and 2nd best column
            costs = xt::row(costMatrix, rowFree) - colRedux;

            // Update the min-valued column's reduction value according to
            // the bid price
            isUnassigned = xt::isnan(colSoln);
            computeBid(costs, isUnassigned, bid, bestCol);
            bid = bid + epsilon;
            colRedux.at(bestCol) = colRedux.at(bestCol) - bid;

            // Assign this row to the min - valued column
            rowPrev = colSoln.at(bestCol);
            colSoln.at(bestCol) = rowFree;
            rowSoln.at(rowFree) = bestCol;

            // If the minimum column was previously assigned to a row, then
            // unassign it
            if (!std::isnan(rowPrev))
            {
                rowSoln.at(rowPrev) = nan;
            }
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