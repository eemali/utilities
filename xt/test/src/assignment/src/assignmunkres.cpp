#include "../include/assignmunkres.h"
#include "../include/lapCheckCostMatrix.h"
#include "../include/lapCheckUnassignedCost.h"
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    static bool isempty(const xt::xarray<size_t> &arr);
    static void cvalgAssignDetectionsToTracks(xt::xarray<double> &cost,
                                              xt::xarray<double> &costUnmatchedTracks,
                                              xt::xarray<double> &costUnmatchedDetections,
                                              xt::xarray<size_t> &matches,
                                              xt::xarray<size_t> &unmatchedTracks,
                                              xt::xarray<size_t> &unmatchedDetections);
    static xt::xarray<double> getPaddedCost(xt::xarray<double> &cost,
                                            xt::xarray<double> &costUnmatchedTracks,
                                            xt::xarray<double> &costUnmatchedDetectionsVector);
    static xt::xarray<bool> hungarianAssignment(xt::xarray<double> &cost);
    static xt::xarray<bool> makeInitialAssignment(xt::xarray<double> &cost);
    static void findNonCoveredZero(xt::xarray<bool> &Z, int &zi, int &zj);
    static void createNewZero(xt::xarray<double> &cost,
                              xt::xarray<bool> &rowCover,
                              xt::xarray<bool> &colCover,
                              xt::xarray<bool> &Z);
    static void alternatePrimesAndStars(xt::xarray<bool> &stars,
                                        xt::xarray<bool> &primes,
                                        int zi, int zj);

    void assignmunkres(xt::xarray<double> costMatrix,
                       double costOfNonAssignment,
                       xt::xarray<size_t> &assignments,
                       xt::xarray<size_t> &unassignedRows,
                       xt::xarray<size_t> &unassignedColumns)
    {
        lapCheckCostMatrix(costMatrix);
        lapCheckUnassignedCost(costOfNonAssignment);

        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        if (0 == nRow || 0 == nCol) // Nothing to assign
        {
            assignments = xt::xarray<size_t>::from_shape({0, 2});
            unassignedRows = xt::arange<size_t>(nRow);
            unassignedColumns = xt::arange<size_t>(nCol);
            return;
        }

        xt::xarray<double> costUnmatchedTracksVector = xt::xarray<double>({nRow}, costOfNonAssignment);
        xt::xarray<double> costUnmatchedDetectionsVector = xt::xarray<double>({nCol}, costOfNonAssignment);

        cvalgAssignDetectionsToTracks(costMatrix, costUnmatchedTracksVector,
                                      costUnmatchedDetectionsVector,
                                      assignments, unassignedRows, unassignedColumns);
    }

    static bool isempty(const xt::xarray<size_t> &arr)
    {
        return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
    }

    static void cvalgAssignDetectionsToTracks(xt::xarray<double> &cost,
                                              xt::xarray<double> &costUnmatchedTracks,
                                              xt::xarray<double> &costUnmatchedDetections,
                                              xt::xarray<size_t> &matches,
                                              xt::xarray<size_t> &unmatchedTracks,
                                              xt::xarray<size_t> &unmatchedDetections)
    {
        // add dummy rows and columns to account for the possibility of
        // unassigned tracks and observations
        xt::xarray<double> paddedCost = getPaddedCost(cost, costUnmatchedTracks, costUnmatchedDetections);

        // solve the assignment problem
        xt::xarray<size_t> idx = xt::from_indices(xt::argwhere(hungarianAssignment(paddedCost)));
        xt::xarray<size_t> rowInds = xt::col(idx, 0);
        xt::xarray<size_t> colInds = xt::col(idx, 1);

        size_t rows = cost.shape(0);
        size_t cols = cost.shape(1);

        unmatchedTracks = xt::filter(rowInds, rowInds < rows & colInds >= cols);
        unmatchedDetections = xt::filter(colInds, colInds < cols & rowInds >= rows);

        matches = xt::stack(xt::xtuple(rowInds, colInds), 1);
        matches = xt::view(matches,
                           xt::keep(xt::flatten_indices(xt::argwhere(rowInds < rows & colInds < cols))),
                           xt::all());
        if (isempty(matches))
        {
            matches = xt::xarray<size_t>::from_shape({0, 2});
        }
    }

    static xt::xarray<double> getPaddedCost(xt::xarray<double> &cost,
                                            xt::xarray<double> &costUnmatchedTracks,
                                            xt::xarray<double> &costUnmatchedDetectionsVector)
    {
        // replace infinities with the biggest possible number
        double bigNumber = std::numeric_limits<double>::max();
        xt::filtration(cost, xt::isinf(cost)) = bigNumber;

        // create a "padded" cost matrix, with dummy rows and columns
        // to account for the possibility of not matching
        size_t rows = cost.shape(0);
        size_t cols = cost.shape(1);
        size_t paddedSize = rows + cols;
        xt::xarray<double> paddedCost = xt::xarray<double>({paddedSize, paddedSize}, bigNumber);

        xt::view(paddedCost, xt::range(_, rows), xt::range(_, cols)) = cost;

        for (size_t i : xt::arange<size_t>(rows))
        {
            paddedCost.at(i, cols + i) = costUnmatchedTracks.at(i);
        }
        for (size_t i : xt::arange<size_t>(cols))
        {
            paddedCost.at(rows + i, i) = costUnmatchedDetectionsVector.at(i);
        }
        xt::view(paddedCost, xt::range(rows, _), xt::range(cols, _)) = 0.0;
        return paddedCost;
    }

    static xt::xarray<bool> hungarianAssignment(xt::xarray<double> &cost)
    {
        size_t rows = cost.shape(0);
        size_t cols = cost.shape(1);

        // step 1: subtract row minima
        xt::xarray<double> row_minina = xt::nanmin(cost, {1});
        cost = cost - row_minina.reshape({-1, 1});

        // step 2: make an initial assignment by "starring" zeros
        xt::xarray<bool> stars = makeInitialAssignment(cost);

        // step 3: cover all columns containing starred zeros
        auto any_functor = xt::make_xreducer_functor([](bool a, bool b) { return a | b; }, xt::const_value<bool>(false));
        xt::xarray<bool> colCover = xt::reduce(any_functor, stars, {0});

        xt::xarray<bool> rowCover, primes, Z, starredRow;
        bool shouldCreateNewZero;
        int zi, zj;
        while (!xt::all(colCover))
        {
            // uncover all rows and unprime all zeros
            rowCover = xt::xarray<bool>({rows}, false);
            primes = xt::xarray<bool>({rows, cols}, false);
            Z = !cost; // mark locations of the zeros
            xt::view(Z, xt::all(), xt::keep(xt::flatten_indices(xt::argwhere(colCover)))) = false;
            while (true)
            {
                shouldCreateNewZero = true;
                // step 4: Find a noncovered zero and prime it.
                findNonCoveredZero(Z, zi, zj);
                while (zi >= 0)
                {
                    primes.at(zi, zj) = true;
                    // find a starred zero in the column containing the primed zero
                    starredRow = xt::row(stars, zi);
                    if (xt::any(starredRow))
                    {
                        // if there is one, cover the its row and uncover
                        // its column.
                        rowCover.at(zi) = true;
                        xt::filtration(colCover, starredRow) = false;
                        xt::row(Z, zi) = false;
                        xt::view(Z,
                                 xt::keep(xt::flatten_indices(xt::argwhere(!rowCover))),
                                 xt::keep(xt::flatten_indices(xt::argwhere(starredRow)))) = !xt::view(cost,
                                                                                                      xt::keep(xt::flatten_indices(xt::argwhere(!rowCover))),
                                                                                                      xt::keep(xt::flatten_indices(xt::argwhere(starredRow))));
                        findNonCoveredZero(Z, zi, zj);
                    }
                    else
                    {
                        shouldCreateNewZero = false;
                        // go to step 5
                        break;
                    }
                }
                if (shouldCreateNewZero)
                {
                    // step 6: create a new zero
                    createNewZero(cost, rowCover, colCover, Z);
                }
                else
                {
                    break;
                }
            }
            // step 5: Construct a series of alternating primed and starred zeros.
            alternatePrimesAndStars(stars, primes, zi, zj);
            // step 3: cover all columns containing starred zeros
            colCover = xt::reduce(any_functor, stars, {0});
        }

        return stars;
    }

    static xt::xarray<bool> makeInitialAssignment(xt::xarray<double> &cost)
    {
        size_t rows = cost.shape(0);
        size_t cols = cost.shape(1);

        xt::xarray<bool> rowCover = xt::xarray<bool>({rows}, false);
        xt::xarray<bool> colCover = xt::xarray<bool>({cols}, false);
        xt::xarray<bool> stars = xt::xarray<bool>({rows, cols}, false);

        xt::xarray<size_t> idx = xt::from_indices(xt::argwhere(xt::equal(cost, 0)));
        xt::xarray<size_t> zr = xt::col(idx, 0);
        xt::xarray<size_t> zc = xt::col(idx, 1);
        for (size_t i : xt::arange(zr.size()))
        {
            if (!rowCover.at(zr.at(i)) && !colCover.at(zc.at(i)))
            {
                stars.at(zr.at(i), zc.at(i)) = true;
                rowCover.at(zr.at(i)) = true;
                colCover.at(zc.at(i)) = true;
            }
        }
        return stars;
    }

    static void findNonCoveredZero(xt::xarray<bool> &Z, int &zi, int &zj)
    {
        xt::xarray<size_t> idx = xt::from_indices(xt::argwhere(Z));
        if (isempty(idx))
        {
            zi = -1;
            zj = -1;
        }
        else
        {
            zi = idx(0, 0);
            zj = idx(0, 1);
        }
    }

    static void createNewZero(xt::xarray<double> &cost,
                              xt::xarray<bool> &rowCover,
                              xt::xarray<bool> &colCover,
                              xt::xarray<bool> &Z)
    {
        size_t rows = cost.shape(0);
        size_t cols = cost.shape(1);
        Z = xt::xarray<bool>({rows, cols}, false);

        // find a minimum uncovered value
        xt::xarray<double> uncovered = xt::view(cost,
                                                xt::keep(xt::flatten_indices(xt::argwhere(!rowCover))),
                                                xt::keep(xt::flatten_indices(xt::argwhere(!colCover))));
        double minVal = xt::nanmin(uncovered)();

        //add the minimum value to all intersections of covered rows and cols
        xt::view(cost,
                 xt::keep(xt::flatten_indices(xt::argwhere(rowCover))),
                 xt::keep(xt::flatten_indices(xt::argwhere(colCover)))) += minVal;

        // subtract the minimum value from all uncovered entries creating at
        // least one new zero

        xt::view(cost,
                 xt::keep(xt::flatten_indices(xt::argwhere(!rowCover))),
                 xt::keep(xt::flatten_indices(xt::argwhere(!colCover)))) = uncovered - minVal;

        //mark locations of all uncovered zeros
        xt::view(Z,
                 xt::keep(xt::flatten_indices(xt::argwhere(!rowCover))),
                 xt::keep(xt::flatten_indices(xt::argwhere(!colCover)))) = !xt::view(cost,
                                                                                     xt::keep(xt::flatten_indices(xt::argwhere(!rowCover))),
                                                                                     xt::keep(xt::flatten_indices(xt::argwhere(!colCover))));
    }

    static void alternatePrimesAndStars(xt::xarray<bool> &stars,
                                        xt::xarray<bool> &primes,
                                        int zi, int zj)
    {
        size_t nRows = stars.shape(0);
        size_t nCols = stars.shape(1);

        // create a logical index of Z0
        xt::xarray<bool> lzi = xt::xarray<bool>({nRows}, false);
        xt::xarray<bool> lzj = xt::xarray<bool>({nCols}, false);
        lzi.at(zi) = true;
        lzj.at(zj) = true;

        // find a starred zero Z1 in the column of Z0
        xt::xarray<bool> rowInd = xt::view(stars,
                                           xt::all(),
                                           xt::keep(xt::flatten_indices(xt::argwhere(lzj))));

        // star Z0
        xt::view(stars,
                 xt::keep(xt::flatten_indices(xt::argwhere(lzi))),
                 xt::keep(xt::flatten_indices(xt::argwhere(lzj)))) = true;

        xt::xarray<bool> llzj;
        while (xt::any(rowInd))
        {
            // unstar Z1
            xt::view(stars,
                     xt::keep(xt::flatten_indices(xt::argwhere(rowInd))),
                     xt::keep(xt::flatten_indices(xt::argwhere(lzj)))) = false;

            // find a primed zero Z2 in Z1's row
            llzj = xt::view(stars,
                            xt::keep(xt::flatten_indices(xt::argwhere(rowInd))),
                            xt::all());
            lzj = xt::row(llzj, 0);
            lzi = rowInd;

            // find a starred zero in Z2's column
            rowInd = xt::view(stars,
                              xt::all(),
                              xt::keep(xt::flatten_indices(xt::argwhere(lzj))));

            // star Z2
            xt::view(stars,
                     xt::keep(xt::flatten_indices(xt::argwhere(lzi))),
                     xt::keep(xt::flatten_indices(xt::argwhere(lzj)))) = true;
        }
    }
}