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

    xt::xarray<bool> isinf_costMatrix = xt::isinf(costMatrix);
    auto all_functor = xt::make_xreducer_functor([](bool a, bool b) { return a & b; }, xt::const_value<bool>(true));
    xt::xarray<bool> isUnassignedRows = xt::reduce(all_functor, isinf_costMatrix, {1});
    xt::xarray<bool> isUnassignedColumns = xt::reduce(all_functor, isinf_costMatrix, {0});

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
void lapColumnReduction(xt::xarray<double> &costMatrix,
                        xt::xarray<double> &rowSoln,
                        xt::xarray<double> &colSoln,
                        xt::xarray<double> &colRedux,
                        xt::xarray<size_t> &rowAssignedCnt)
{
    // For square cost matrices (nRows == nColumns), the first step in JV
    // Assignment assigns columns to unassigned rows with the least cost
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
    rowAssignedCnt = xt::zeros<size_t>({nRow});

    size_t rowMin;
    double costMin, colAssigned;
    // Starting with the last column, find row in each column with the minimum
    // cost. If the min-valued row is not already assigned, assign that row to
    // the column
    for (size_t col : xt::view(xt::arange<size_t>(nCol), xt::range(_, _, -1)))
    {
        // Find min-valued row
        rowMin = xt::argmin(xt::col(costMatrix, col))();
        costMin = costMatrix.at(rowMin, col);

        // Update column reduction
        colRedux.at(col) = costMin;

        // Update the number of times this row was the minimum
        rowAssignedCnt.at(rowMin) = rowAssignedCnt.at(rowMin) + 1;

        // Assign row to column, if not already assigned
        colAssigned = rowSoln.at(rowMin);

        if (std::isnan(colAssigned))
        {
            rowSoln.at(rowMin) = col;
            colSoln.at(col) = rowMin;
        }
        else if (costMin < colRedux.at(colAssigned))
        {
            // If the column this row was previously assigned to had a higher
            // cost than this new column, then reassign it
            rowSoln.at(rowMin) = col;
            colSoln.at(col) = rowMin;
            colSoln.at(colAssigned) = std::numeric_limits<double>::quiet_NaN();
        }
    }
}
void lapReductionTransfer(const xt::xarray<double> &costMatrix,
                          const xt::xarray<double> &rowSoln,
                          xt::xarray<double> &colRedux,
                          const xt::xarray<size_t> &rowAssignedCnt)
{
    // For square cost matrices (nRows == nColumns), the second step in JV
    // Assignment enables the next step (Augmenting Row Reduction) by adjusting
    // the column reduction values to ensure that the costs across each row will
    // not include negative values

    // Update column reduction values for rows that were assigned only once
    xt::xarray<size_t> assignedOnceRows = xt::flatten_indices(xt::argwhere(!xt::isnan(rowSoln) & xt::equal(rowAssignedCnt, 1)));
    xt::xarray<double> rowCost;
    double costMin, colAssigned;
    for (size_t row : assignedOnceRows)
    {
        // Find minimum value for the column not assigned to this row
        rowCost = xt::row(costMatrix, row) - colRedux;
        colAssigned = rowSoln.at(row);
        rowCost.at(colAssigned) = std::numeric_limits<double>::quiet_NaN();
        costMin = xt::amin(rowCost)();
        // Update the column reduction by this minimum value
        colRedux.at(colAssigned) = colRedux.at(colAssigned) - costMin;
    }
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
             xt::xarray<double> &colRedux)
{
    // Performs one iteration of the forward Auction linear assignment algorithm

    // Each Auction iteration cycles over all free rows
    xt::xarray<size_t> rowList = xt::flatten_indices(xt::argwhere(xt::isnan(rowSoln)));
    xt::xarray<double> costs;
    xt::xarray<bool> isUnassigned;
    size_t bestCol;
    double bid, rowPrev;
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
            rowSoln.at(rowPrev) = std::numeric_limits<double>::quiet_NaN();
        }
    }
}
void lapAuction(xt::xarray<double> &costMatrix,
                xt::xarray<double> &rowSoln,
                xt::xarray<double> &colSoln,
                xt::xarray<double> &colRedux,
                double maxAuctions = std::numeric_limits<double>::infinity(),
                bool hasEpsilonScaling = false)
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
    double epsFactor = 7;
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
            auction(costMatrix, epsilon, rowSoln, colSoln, colRedux);
            auctionCount = auctionCount + 1;
        }

        // Update scaling
        epsilon = std::max(epsilon / epsFactor, minBid);
    }
}

void lapDijkstra(xt::xarray<double> &costMatrix,
                 xt::xarray<double> &rowSoln,
                 xt::xarray<double> &colSoln,
                 xt::xarray<double> &colRedux)
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

    double distMin, costOld;
    size_t low, up, last, endOfPath, nMin, colMin, rowOld, rowAssigned, colAssigned;
    bool unassignedFound, noMinFound;
    xt::xarray<double> dist, costNew, edgeLen, distAdj;
    xt::xarray<size_t> prevRow, colList, tmp, k, kMin, colNew, colReady;
    xt::xarray<bool> isMin, isUnassignedCol, doRelax, isMinLen, isTerminal;

    xt::xarray<size_t> freeRows = xt::flatten_indices(xt::argwhere(xt::isnan(rowSoln)));
    for (size_t rowFree : freeRows)
    {
        // cost of assigning this row to each col
        dist = xt::row(costMatrix, rowFree) - colRedux;

        // Do any valid distances exist for this row?
        if (!xt::all(xt::isinf(dist)))
        {
            // Paths alternate between rows and cols, prevRow keeps track of each
            // column's previous row
            prevRow = xt::xarray<size_t>({nCol}, rowFree); // Set previous row in tree as this row

            // colList places columns in 1 of 3 queues. 'low' and 'up' partition
            // colList according to the table below. Its entries are indices into
            // colSoln
            //   [1:low-1]   : columns ready to have their assignment finalized
            //   [low:up-1]  : columns to be scanned for *current* minimum
            //   [up:n]      : columns to be scanned later for a *new* minimum
            low = 0;
            up = 0;
            colList = xt::arange<size_t>(nCol);
            last = 0;
            endOfPath = 0;

            unassignedFound = false;
            noMinFound = false;
            distMin = 0.0;

            while (!unassignedFound)
            {
                // If no min-valued columns exist, search for a new set
                if (low == up)
                {
                    last = low;
                    // Find all columns with min value and place them in the
                    // 'current minimum' queue
                    k = xt::arange<size_t>(up, nCol);
                    // distMin = min(dist(colList(k)));
                    distMin = xt::nanmin(xt::view(dist, xt::keep(xt::view(colList, xt::keep(k)))))();

                    // Skip to the next row since there are no min-valued
                    // columns left for this row
                    if (std::isinf(distMin))
                    {
                        noMinFound = true;
                        break;
                    }

                    isMin = xt::equal(xt::view(dist, xt::keep(xt::view(colList, xt::keep(k)))), distMin);
                    kMin = xt::filter(k, isMin);

                    // Check if any of the min-valued columns correspond to an
                    // unassigned column, if so the alternating path is complete
                    isUnassignedCol = xt::isnan(xt::view(colSoln, xt::keep(xt::view(colList, xt::keep(kMin)))));
                    if (xt::any(isUnassignedCol))
                    {
                        endOfPath = xt::view(colList, xt::keep(xt::filter(kMin, isUnassignedCol)))(0);
                        unassignedFound = true;
                    }
                    else
                    {
                        // Swap values to place in queue and avoid overwrite
                        nMin = xt::sum(isMin)();

                        tmp = xt::view(colList, xt::keep(k));
                        xt::view(colList, xt::keep(xt::arange<size_t>(nMin) + up)) = xt::filter(tmp, isMin);
                        up = up + nMin;
                        xt::view(colList, xt::keep(xt::arange<size_t>(xt::sum(!isMin)()) + up)) = xt::filter(tmp, !isMin);
                    }
                }

                if (!unassignedFound)
                {
                    // Use a modified version of Dijkstra's algorithm to find the
                    // path alternating between rows and columns with the least
                    // total cost of assignment
                    //
                    // This maps into Dijkstra's algorithm as follows:
                    //
                    // - The source vertex is the new assignment: (rowFree, colMin)
                    // - The distance of the source is its cost
                    // - The neighboring vertices are the assignments of the
                    //   currently assigned row (rowOld) with all of the
                    //   non-min-valued columns (newCol)
                    // - The distance of each neighbor is its cost
                    // - The length of the edge connecting a neighbor to the source
                    //   is the difference in cost between rowOld and rowFree
                    //
                    // The path terminates when an unassigned column is found

                    colMin = colList.at(low);
                    low = low + 1; // Search next min-valued column on next loop

                    rowOld = colSoln.at(colMin); // Currently assigned row
                    costOld = costMatrix.at(rowOld, colMin) - colRedux.at(colMin);

                    k = xt::arange<size_t>(up, nCol);
                    colNew = xt::view(colList, xt::keep(k));
                    costNew = xt::view(costMatrix, rowOld, xt::keep(colNew)) - xt::view(colRedux, xt::keep(colNew));

                    // Compute edge lengths
                    edgeLen = costNew - costOld;

                    // Apply relaxation
                    distAdj = edgeLen + distMin;
                    doRelax = distAdj < xt::view(dist, xt::keep(colNew));
                    // dist(colNew(doRelax)) = distAdj(doRelax);
                    xt::view(dist, xt::keep(xt::filter(colNew, doRelax))) = xt::filter(distAdj, doRelax);
                    // prevRow(colNew(doRelax)) = rowOld;
                    xt::view(prevRow, xt::keep(xt::filter(colNew, doRelax))) = rowOld;

                    // Check for terminal condition for the alternating path for any
                    // of the relaxed columns
                    isMinLen = xt::equal(distAdj, distMin) & doRelax;
                    isTerminal = xt::isnan(xt::view(colSoln, xt::keep(colNew))) & isMinLen;

                    if (xt::any(isTerminal))
                    {
                        endOfPath = xt::filter(colNew, isTerminal)[0];
                        unassignedFound = true;
                    }
                    else if (xt::any(isMinLen))
                    {
                        // Terminal condition not met, add relaxed columns to be
                        // scanned on next loop

                        // Swap values to place in queue to avoid overwrite
                        nMin = xt::sum(isMinLen)();

                        tmp = xt::view(colList, xt::keep(k));
                        xt::view(colList, xt::keep(xt::arange<size_t>(nMin) + up)) = xt::filter(tmp, isMinLen);
                        up = up + nMin;
                        xt::view(colList, xt::keep(xt::arange<size_t>(xt::sum(!isMinLen)()) + up)) = xt::filter(tmp, !isMinLen);
                    }
                }
            }
            if (!noMinFound)
            {
                // Update column reduction values based on new assignment
                colReady = xt::view(colList, xt::keep(xt::arange<size_t>(last)));
                xt::view(colRedux, xt::keep(colReady)) += xt::view(dist, xt::keep(colReady)) - distMin;

                // Set row and column assignments based on shortest path starting at the
                // end of the path and marching backwards until rowFree is reached
                while (true)
                {
                    rowAssigned = prevRow.at(endOfPath);
                    colAssigned = endOfPath;

                    colSoln.at(endOfPath) = rowAssigned;
                    endOfPath = rowSoln.at(rowAssigned);
                    rowSoln.at(rowAssigned) = colAssigned;

                    if (rowAssigned == rowFree)
                    {
                        break;
                    }
                }
            }
        }
    }
}
void assignjv(xt::xarray<double> costMatrix,
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
    lapColumnReduction(costMatrix, rowSoln, colSoln, colRedux, rowAssignedCnt);
    lapReductionTransfer(costMatrix, rowSoln, colRedux, rowAssignedCnt);
    lapAuction(costMatrix, rowSoln, colSoln, colRedux, 2, false);
    lapDijkstra(costMatrix, rowSoln, colSoln, colRedux);
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
    assignjv(costMatrix, costOfNonAssignment, assignments, unassignedTracks, unassignedDetections);
    std::cout << assignments << std::endl;
    std::cout << unassignedTracks << std::endl;
    std::cout << unassignedDetections << std::endl;
    return 0;
}
