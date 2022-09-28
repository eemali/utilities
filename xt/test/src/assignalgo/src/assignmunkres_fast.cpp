#include "../include/assignmunkres_fast.h"
#include "../internal/include/assigntypedef.h"
#include "../internal/include/lapCheckCostMatrix.h"
#include "../internal/include/lapCheckUnassignedCost.h"
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    // Supporting Functions
    // --------------------
    static std::tuple<xt::xarray<size_t>,
                      xt::xarray<size_t>,
                      xt::xarray<size_t>>
    cvalgAssignDetectionsToTracks(xt::xarray<double> &cost,
                                  const xt::xarray<double> &costUnmatchedTracks,
                                  const xt::xarray<double> &costUnmatchedDetections);
    static xt::xarray<double> getPaddedCost(xt::xarray<double> &cost,
                                            const xt::xarray<double> &costUnmatchedTracks,
                                            const xt::xarray<double> &costUnmatchedDetectionsVector);
    static xt::xarray<bool> hungarianAssignment(xt::xarray<double> &cost);
    static void find_a_zero(const xt::xarray<double> &costMatrix,
                            const xt::xarray<bool> &rowCover,
                            const xt::xarray<bool> &colCover,
                            int &zi, int &zj);
    static int find_star_in_row(const xt::xarray<uint8_t> &stars, int i);
    static int find_star_in_col(const xt::xarray<uint8_t> &stars, int j);
    static double find_smallest(const xt::xarray<double> &costMatrix,
                                const xt::xarray<bool> &rowCover,
                                const xt::xarray<bool> &colCover);
    static int find_prime_in_row(const xt::xarray<uint8_t> &stars, int i);

    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    assignmunkres_fast(xt::xarray<double> costMatrix,
                       double costOfNonAssignment)
    {
        // check inputs
        lapCheckCostMatrix(costMatrix);
        lapCheckUnassignedCost(costOfNonAssignment);

        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        if (0 == nRow || 0 == nCol) // Nothing to assign
        {
            xt::xarray<size_t> assignments = xt::xarray<size_t>::from_shape({0, 2});
            xt::xarray<size_t> unassignedRows = xt::arange<size_t>(nRow);
            xt::xarray<size_t> unassignedColumns = xt::arange<size_t>(nCol);
            return std::make_tuple(std::move(assignments),
                                   std::move(unassignedRows),
                                   std::move(unassignedColumns));
        }

        xt::xarray<double> costUnmatchedTracksVector = xt::xarray<double>({nRow}, costOfNonAssignment);
        xt::xarray<double> costUnmatchedDetectionsVector = xt::xarray<double>({nCol}, costOfNonAssignment);

        return cvalgAssignDetectionsToTracks(costMatrix, costUnmatchedTracksVector,
                                             costUnmatchedDetectionsVector);
    }

    static std::tuple<xt::xarray<size_t>,
                      xt::xarray<size_t>,
                      xt::xarray<size_t>>
    cvalgAssignDetectionsToTracks(xt::xarray<double> &cost,
                                  const xt::xarray<double> &costUnmatchedTracks,
                                  const xt::xarray<double> &costUnmatchedDetections)
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

        xt::xarray<size_t> unmatchedTracks = xt::filter(rowInds, rowInds < rows & colInds >= cols);
        xt::xarray<size_t> unmatchedDetections = xt::filter(colInds, colInds < cols & rowInds >= rows);

        xt::xarray<size_t> matches = xt::stack(xt::xtuple(rowInds, colInds), 1);
        matches = xt::view(matches,
                           xt::keep(xt::flatten_indices(xt::argwhere(rowInds < rows & colInds < cols))),
                           xt::all());
        if (isempty(matches))
        {
            matches = xt::xarray<size_t>::from_shape({0, 2});
        }
        return std::make_tuple(std::move(matches),
                               std::move(unmatchedTracks),
                               std::move(unmatchedDetections));
    }

    static xt::xarray<double> getPaddedCost(xt::xarray<double> &cost,
                                            const xt::xarray<double> &costUnmatchedTracks,
                                            const xt::xarray<double> &costUnmatchedDetectionsVector)
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
    static xt::xarray<bool> hungarianAssignment(xt::xarray<double> &costMatrix)
    {
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        xt::xarray<uint8_t> stars = xt::zeros<uint8_t>({nRow, nCol});
        xt::xarray<bool> rowCover = xt::xarray<bool>({nRow}, false);
        xt::xarray<bool> colCover = xt::xarray<bool>({nCol}, false);

        // step 1: subtract row minima
        xt::xarray<double> row_minima = xt::nanmin(costMatrix, {1});
        costMatrix = costMatrix - row_minima.reshape({-1, 1});

        // step 2: make an initial assignment by "starring" zeros
        double eps = std::numeric_limits<double>::epsilon();
        for (size_t i = 0; i < nRow; ++i)
        {
            for (size_t j = 0; j < nCol; ++j)
            {
                if (std::abs(costMatrix.at(i, j)) < eps) // e.g (0 == costMatrix.at(i, j))
                {
                    if (!rowCover.at(i) && !colCover.at(j))
                    {
                        stars.at(i, j) = 1;
                        rowCover.at(i) = true;
                        colCover.at(j) = true;
                    }
                }
            }
        }
        rowCover.fill(false);
        colCover.fill(false);

        size_t step = 3;
        std::vector<int> last_point;
        bool done = false;
        while (!done)
        {
            switch (step)
            {
            case 3:
            {
                int nColCovered = 0;
                for (size_t j = 0; j < nCol; ++j)
                {
                    for (size_t i = 0; i < nRow; ++i)
                    {
                        if (1 == stars.at(i, j))
                        {
                            colCover.at(j) = true;
                            ++nColCovered;
                            break;
                        }
                    }
                }
                if ((nColCovered >= nCol) || (nColCovered >= nRow))
                {
                    step = -1;
                }
                else
                {
                    step = 4;
                }
            }
            break;
            case 4:
            {
                int zi, zj, j;
                while (true)
                {
                    find_a_zero(costMatrix, rowCover, colCover, zi, zj);
                    if (-1 == zi)
                    {
                        step = 6;
                        break;
                    }
                    else
                    {
                        stars.at(zi, zj) = 2;
                        j = find_star_in_row(stars, zi);
                        if (j > -1)
                        {
                            zj = j;
                            rowCover.at(zi) = true;
                            colCover.at(zj) = false;
                        }
                        else
                        {
                            step = 5;
                            last_point = {zi, zj};
                            break;
                        }
                    }
                }
            }
            break;
            case 5:
            {
                std::vector<std::vector<int>> path = {last_point};
                int i;
                while (true)
                {
                    i = find_star_in_col(stars, last_point[1]);
                    if (i > -1)
                    {
                        last_point[0] = i;
                        path.push_back(last_point);
                    }
                    else
                    {
                        break;
                    }
                    last_point[1] = find_prime_in_row(stars, last_point[0]);
                    path.push_back(last_point);
                }
                // augment path
                for (auto p : path)
                {
                    if (1 == stars.at(p[0], p[1]))
                    {
                        stars.at(p[0], p[1]) = 0;
                    }
                    else
                    {
                        stars.at(p[0], p[1]) = 1;
                    }
                }
                // clear covers
                rowCover.fill(false);
                colCover.fill(false);
                // erase primes
                std::for_each(stars.begin(), stars.end(),
                              [](uint8_t &v)
                              {
                                  if (2 == v)
                                  {
                                      v = 0;
                                  }
                              });
                step = 3;
            }
            break;
            case 6:
            {
                double minval = find_smallest(costMatrix, rowCover, colCover);
                if (minval == std::numeric_limits<double>::infinity())
                {
                    step = -1;
                }
                else
                {
                    for (int i = 0; i < nRow; ++i)
                    {
                        for (int j = 0; j < nCol; ++j)
                        {
                            if (rowCover.at(i))
                            {
                                costMatrix.at(i, j) += minval;
                            }
                            if (!colCover.at(j))
                            {
                                costMatrix.at(i, j) -= minval;
                            }
                        }
                    }
                    step = 4;
                }
            }
            break;
            default:
                done = true;
                break;
            }
        }
        return xt::equal(stars, 1);
    }
    static void find_a_zero(const xt::xarray<double> &costMatrix,
                            const xt::xarray<bool> &rowCover,
                            const xt::xarray<bool> &colCover,
                            int &zi, int &zj)
    {
        zi = -1;
        zj = -1;
        int i = 0;
        int j;
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);
        bool done = false;
        while (!done)
        {
            j = 0;
            while (true)
            {
                if ((0 == costMatrix.at(i, j)) && (!rowCover.at(i)) && (!colCover.at(j)))
                {
                    zi = i;
                    zj = j;
                    done = true;
                }
                j += 1;
                if ((j >= nCol) || (done))
                {
                    break;
                }
            }
            i += 1;
            if (i >= nRow)
            {
                done = true;
            }
        }
    }
    static int find_star_in_row(const xt::xarray<uint8_t> &stars, int i)
    {
        size_t nCol = stars.shape(1);
        for (int j = 0; j < nCol; ++j)
        {
            if (1 == stars.at(i, j))
            {
                return j;
            }
        }
        return -1;
    }
    static int find_star_in_col(const xt::xarray<uint8_t> &stars, int j)
    {
        size_t nRow = stars.shape(0);
        for (int i = 0; i < nRow; ++i)
        {
            if (1 == stars.at(i, j))
            {
                return i;
            }
        }
        return -1;
    }
    static double find_smallest(const xt::xarray<double> &costMatrix,
                                const xt::xarray<bool> &rowCover,
                                const xt::xarray<bool> &colCover)
    {
        double minval = std::numeric_limits<double>::infinity();
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);
        for (int i = 0; i < nRow; ++i)
        {
            if (!rowCover.at(i))
            {
                for (int j = 0; j < nCol; ++j)
                {
                    if (!colCover.at(j))
                    {
                        if (minval > costMatrix.at(i, j))
                        {
                            minval = costMatrix.at(i, j);
                        }
                    }
                }
            }
        }
        return minval;
    }
    static int find_prime_in_row(const xt::xarray<uint8_t> &stars, int i)
    {
        size_t nCol = stars.shape(1);
        for (int j = 0; j < nCol; ++j)
        {
            if (2 == stars.at(i, j))
            {
                return j;
            }
        }
        return -1;
    }
}