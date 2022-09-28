#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>
using namespace xt::placeholders;
template <typename T>
bool isempty(xt::xarray<T> arr)
{
    return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
}
void lapRemoveImpossibles(xt::xarray<double> &costMatrix,
                          double costOfNonAssignment,
                          xt::xarray<size_t> &rowIdx,
                          xt::xarray<size_t> &colIdx,
                          xt::xarray<size_t> &unassignedRows,
                          xt::xarray<size_t> &unassignedColumns)
{
    // Gate values in the cost matrix which are greater-than or equal-to twice
    // the cost of non-assignment
    xt::filtration(costMatrix, costMatrix >= 2 * costOfNonAssignment) = std::numeric_limits<double>::infinity();

    // Remove gated (set to inf) rows or columns in the costMatrix where no
    // assignment is possibl
    size_t nRow = costMatrix.shape(0);
    size_t nCol = costMatrix.shape(1);

    rowIdx = xt::arange<size_t>(nRow);
    colIdx = xt::arange<size_t>(nCol);

    auto all_functor = xt::make_xreducer_functor([](bool a, bool b) { return a & b; }, xt::const_value<bool>(true));
    xt::xarray<bool> isUnassignedRows = xt::reduce(all_functor, xt::isinf(costMatrix), {1});
    xt::xarray<bool> isUnassignedColumns = xt::reduce(all_functor, xt::isinf(costMatrix), {0});

    unassignedRows = xt::flatten_indices(xt::argwhere(isUnassignedRows));
    unassignedColumns = xt::flatten_indices(xt::argwhere(isUnassignedColumns));

    costMatrix = xt::view(costMatrix, xt::drop(unassignedRows), xt::drop(unassignedColumns));
    rowIdx = xt::view(rowIdx, xt::drop(unassignedRows));
    colIdx = xt::view(colIdx, xt::drop(unassignedColumns));
}
void lapPadForUnassignedRowsAndColumns(xt::xarray<double> &costMatrix,
                                       double costOfNonAssignment)
{
    // Pad matrix with dummy rows and columns to include the costOfNonAssignment
    // in the solution
    size_t nRow = costMatrix.shape(0);
    size_t nCol = costMatrix.shape(1);

    xt::xarray<double> dummyCols = xt::xarray<double>({nRow, nRow}, std::numeric_limits<double>::infinity());
    std::vector<xt::xindex> idx1;
    for (size_t idx : xt::arange<size_t>(nRow))
    {
        idx1.push_back(xt::xindex({idx, idx}));
    }
    xt::index_view(dummyCols, idx1) = costOfNonAssignment;
    xt::xarray<double> dummyRows = xt::xarray<double>({nCol, nCol}, std::numeric_limits<double>::infinity());
    std::vector<xt::xindex> idx2;
    for (size_t idx : xt::arange<size_t>(nCol))
    {
        idx2.push_back(xt::xindex({idx, idx}));
    }
    xt::index_view(dummyRows, idx2) = costOfNonAssignment;
    costMatrix = xt::vstack(xtuple(xt::hstack(xtuple(costMatrix, dummyCols)),
                                   xt::hstack(xtuple(dummyRows, xt::zeros<double>({nCol, nRow})))));
}
void computeBid(xt::xarray<double> &costs,
                xt::xarray<bool> &isUnassigned,
                double &bid,
                size_t &bestInd)
{
    // Returns the 2 smallest values and their corresponding indices in costs.
    // When multiple values at the same minimum are found, will return a value
    // corresponding to an unassigned index as the first minimum
    double min1 = xt::amin(costs)(), min2;
    xt::xarray<size_t> minInds = xt::flatten_indices(xt::argwhere(xt::equal(costs, min1)));
    bestInd = minInds.at(0);
    xt::xarray<bool> isMinUnassigned;
    if (minInds.size() > 1)
    {
        minInds = xt::view(minInds, xt::range(1, _));
        isMinUnassigned = xt::index_view(isUnassigned, minInds);
        if (xt::any(isMinUnassigned))
        {
            minInds = xt::index_view(minInds, xt::argwhere(isMinUnassigned));
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
void auction(xt::xarray<double> &costMatrix,
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
    while ((xt::sum(!isUnassigned)() < (nAssigned + 1)) && (priceWarIter < 1000))
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
void lapAuctionFwdRev(xt::xarray<double> &costMatrix,
                      xt::xarray<double> &rowSoln,
                      xt::xarray<double> &colSoln,
                      xt::xarray<double> &colRedux,
                      double maxAuctions = std::numeric_limits<double>::infinity(),
                      bool hasEpsilonScaling = true)
{
    size_t nRow = costMatrix.shape(0);
    size_t nCol = costMatrix.shape(1);

    if (isempty(rowSoln))
    {
        rowSoln = xt::xarray<double>({nRow}, std::numeric_limits<double>::quiet_NaN());
    }
    if (isempty(colSoln))
    {
        colSoln = xt::xarray<double>({nCol}, std::numeric_limits<double>::quiet_NaN());
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
    xt::xarray<double> v = xt::index_view(costMatrix, xt::argwhere(xt::isfinite(costMatrix)));
    double maxRes = xt::amax(v)() - xt::amin(v)();
    double minRes = xt::amin(xt::diff(xt::unique(v)))();
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
        rowSoln = xt::xarray<double>({nRow}, std::numeric_limits<double>::quiet_NaN());
        colSoln = xt::xarray<double>({nCol}, std::numeric_limits<double>::quiet_NaN());

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

void assignauction(xt::xarray<double> costMatrix,
                   double costOfNonAssignment,
                   xt::xarray<size_t> &assignments,
                   xt::xarray<size_t> &unassignedRows,
                   xt::xarray<size_t> &unassignedColumns)
{
    xt::xarray<size_t> rowIdx, colIdx;
    lapRemoveImpossibles(costMatrix, costOfNonAssignment, rowIdx, colIdx, unassignedRows, unassignedColumns);
    size_t nRow = costMatrix.shape(0);
    size_t nCol = costMatrix.shape(1);
    lapPadForUnassignedRowsAndColumns(costMatrix, costOfNonAssignment);
    xt::xarray<double> rowSoln, colSoln, colRedux;
    xt::xarray<size_t> rowAssignedCnt;
    lapAuctionFwdRev(costMatrix, rowSoln, colSoln, colRedux);
    // Remove assignments to padded rows and columns
    rowSoln = xt::view(rowSoln, xt::keep(xt::arange<size_t>(nRow)));
    colSoln = xt::view(colSoln, xt::keep(xt::arange<size_t>(nCol)));
    xt::filtration(rowSoln, rowSoln > nCol) = std::numeric_limits<double>::quiet_NaN();
    xt::filtration(colSoln, colSoln > nRow) = std::numeric_limits<double>::quiet_NaN();
    // Place assignments into the expected return format
    xt::xarray<bool> isRowAssigned = !xt::isnan(rowSoln);
    xt::xarray<bool> isColAssigned = !xt::isnan(colSoln);
    assignments = xt::stack(xt::xtuple(xt::filter(rowIdx, isRowAssigned),
                                       xt::view(colIdx, xt::keep(xt::filter(rowSoln, isRowAssigned)))),
                            1);
    unassignedRows = xt::concatenate(xt::xtuple(unassignedRows,
                                                xt::filter(rowIdx, !isRowAssigned)),
                                     0);
    unassignedColumns = xt::concatenate(xt::xtuple(unassignedColumns,
                                                   xt::filter(colIdx, !isColAssigned)),
                                        0);
}
#include <xtensor/xrandom.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xnorm.hpp>
void str_b(xt::xarray<bool> &b)
{
    std::cout << b << std::endl;
}
void str_s(xt::xarray<size_t> &s)
{
    std::cout << s << std::endl;
}
void str_d(xt::xarray<double> &d)
{
    std::cout << d << std::endl;
}
int main(int argc, char const *argv[])
{
    xt::xarray<double> tracks = {{1, 1}, {2, 2}};
    xt::xarray<double> dets = {{1.1, 1.1}, {2.1, 2.1}, {1.5, 3}};

    xt::xarray<double> costMatrix = xt::empty<double>({tracks.shape(0), dets.shape(0)});
    for (size_t i : xt::view(xt::arange<size_t>(tracks.shape(0)), xt::range(_, _, -1)))
    {
        xt::xarray<double> delta = dets - xt::row(tracks, i);
        xt::row(costMatrix, i) = xt::norm_l2(delta, {1});
    }
#if 1
    costMatrix = {{0.913337361501670, 0.0781755287531837, 0.774910464711502, 0.259870402850654},
                  {0.152378018969223, 0.442678269775446, 0.817303220653433, 0.800068480224308},
                  {0.825816977489547, 0.106652770180584, 0.868694705363510, 0.431413827463545},
                  {0.538342435260057, 0.961898080855054, 0.0844358455109103, 0.910647594429523},
                  {0.996134716626886, 0.00463422413406744, 0.399782649098897, 0.181847028302853}};

    costMatrix = xt::random::rand<double>({1000, 1000});
#endif
    double costOfNonAssignment = 0.2;
    xt::xarray<size_t> assignments, unassignedTracks, unassignedDetections;
    assignauction(costMatrix, costOfNonAssignment, assignments, unassignedTracks, unassignedDetections);
    std::cout << assignments << std::endl;
    std::cout << unassignedTracks << std::endl;
    std::cout << unassignedDetections << std::endl;
    return 0;
}
