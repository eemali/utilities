#include "../include/munkres.h"
#include <xtensor/xview.hpp>
namespace assignalgo
{
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
    void munkres(xt::xarray<double> costMatrix,
                 double costOfNonAssignment,
                 xt::xarray<size_t> &assignments,
                 xt::xarray<size_t> &unassignedRows,
                 xt::xarray<size_t> &unassignedColumns)
    {
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);
        bool hasNegative = xt::any(costMatrix < 0);
        if (0 == nRow || 0 == nCol || hasNegative) // Nothing to assign
        {
            if (hasNegative)
            {
                std::cout << "\x1B[33m"
                          << "Warning : costMat must be non-negative matrix"
                          << "\033[0m" << std::endl;
            }
            assignments = xt::xarray<size_t>::from_shape({0, 2});
            unassignedRows = xt::arange<size_t>(nRow);
            unassignedColumns = xt::arange<size_t>(nCol);
            return;
        }
        // step 0
        bool istransposed = false;
        if (nCol < nRow)
        {
            costMatrix = xt::transpose(costMatrix);
            istransposed = true;
        }
        nRow = costMatrix.shape(0);
        nCol = costMatrix.shape(1);

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
            if (!rowCover.at(i))
            {
                for (size_t j = 0; j < nCol; ++j)
                {
                    if (!colCover.at(j))
                    {
                        if (std::abs(costMatrix.at(i, j)) < eps) // e.g (0 == costMatrix.at(i, j))
                        {
                            stars.at(i, j) = 1;
                            rowCover.at(i) = true;
                            colCover.at(j) = true;
                        }
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
                    step = 7;
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
                              [](uint8_t &v) {if (2 == v){v = 0;} });
                step = 3;
            }
            break;
            case 6:
            {
                double minval = find_smallest(costMatrix, rowCover, colCover);
                if (minval == std::numeric_limits<double>::infinity())
                {
                    step = 7;
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
            case 7:
            {
                if (istransposed)
                {
                    stars = xt::transpose(stars);
                }
                xt::xarray<bool> bstars = xt::equal(stars, 1);
                auto any_functor = xt::make_xreducer_functor([](bool a, bool b) { return a | b; }, xt::const_value<bool>(false));
                unassignedRows = xt::flatten_indices(xt::argwhere(!xt::reduce(any_functor, bstars, {1})));
                unassignedColumns = xt::flatten_indices(xt::argwhere(!xt::reduce(any_functor, bstars, {0})));
                assignments = xt::from_indices(xt::argwhere(bstars));
                step = -1;
            }
            break;
            default:
                done = true;
                break;
            }
        }
    }
}