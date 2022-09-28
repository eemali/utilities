#include "../include/assignAuctionWithPrice.h"
#include "../include/lapAuction.h"
#include "../include/lapRemoveImpossibles.h"
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
namespace assignalgo
{
    void assignAuctionWithPrice(xt::xarray<double> costMatrix,
                                xt::xarray<size_t> &assignments,
                                xt::xarray<size_t> &unassignedRows,
                                xt::xarray<size_t> &unassignedColumns,
                                xt::xarray<double> &price)
    {
        // Assess the size of cost Matrix.
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        // Scale the cost matrix so that padding is not required. We are only
        // concerned about the ratio of prices.
        double eps = std::numeric_limits<double>::epsilon();
        xt::xarray<bool> isfinite_costMatrix = xt::isfinite(costMatrix);
        double maxValue = std::max(eps, xt::nanmax(xt::abs(xt::filter(costMatrix, isfinite_costMatrix)))());
        costMatrix /= maxValue;

        // Cost matrix is now less than 1. 2 is maximum cost of non-assignment for
        // all feasible assignments.
        double smallNumber = 0.1;
        double bigNumber = 2 + smallNumber;

        // Small eps differences can also cause bad scaling. Auction is sub-optimal
        // and this will not affect the quality of solution that we are looking for.
        costMatrix = xt::round(costMatrix * 1e10) / 1e10;

        xt::filter(costMatrix, !isfinite_costMatrix) = xt::sign(xt::filter(costMatrix, !isfinite_costMatrix)) * bigNumber;

        // Assign price before reducing matrix
        price = xt::zeros<double>({nCol});

        xt::xarray<size_t> rowIdx, colIdx;
        xt::xarray<double> checkMatrix = lapRemoveImpossibles(costMatrix, bigNumber,
                                                              rowIdx, colIdx,
                                                              unassignedRows, unassignedColumns);
        if (checkMatrix.shape() != costMatrix.shape())
        {
            assignments = xt::xarray<size_t>::from_shape({0, 2});
            unassignedRows = xt::arange<size_t>(nRow);
            unassignedColumns = xt::arange<size_t>(nCol);
            return;
        }

        // The prices are not guaranteed to be negative for Forward-Reverse auction,
        // which is necessary for S-D assignment.
        // Use the standard auction to compute prices.
        xt::xarray<double> rowSoln, colSoln, priceColn;
        lapAuction(costMatrix, rowSoln, colSoln, priceColn);
        priceColn = priceColn * maxValue;

        // Remove assignments to padded rows and columns
        rowSoln = xt::view(rowSoln, xt::keep(xt::arange<size_t>(nRow)));
        colSoln = xt::view(colSoln, xt::keep(xt::arange<size_t>(nCol)));
        price = xt::view(priceColn, xt::keep(xt::arange<size_t>(nCol)));
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
}