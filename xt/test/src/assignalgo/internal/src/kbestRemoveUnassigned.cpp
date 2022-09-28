#include "../include/kbestRemoveUnassigned.h"
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xsort.hpp>
namespace assignalgo
{
    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    kbestRemoveUnassigned(xt::xarray<size_t> assignment,
                          const xt::xarray<size_t> &costSize)
    {
        size_t nRow = costSize.at(0);
        size_t nCol = costSize.at(1);
        xt::xarray<size_t> rowIdx = xt::arange<size_t>(nRow);
        xt::xarray<double> rowSoln = xt::col(assignment, 1);
        rowSoln = xt::view(rowSoln, xt::keep(rowIdx));
        xt::filtration(rowSoln, rowSoln >= nCol) = std::numeric_limits<double>::quiet_NaN();
        xt::xarray<bool> isRowAssigned = !xt::isnan(rowSoln);

        xt::xarray<size_t> unassignedRows = xt::filter(rowIdx, !isRowAssigned);

        xt::xarray<size_t> colIdx = xt::arange<size_t>(nCol);
		// CAVEAT: sortrows
        xt::xarray<size_t> I = xt::argsort(xt::col(assignment, 1));
        xt::xarray<double> colSoln = xt::view(assignment, xt::keep(I));
        colSoln = xt::view(colSoln, xt::keep(colIdx), 0);
        xt::filtration(colSoln, colSoln >= nRow) = std::numeric_limits<double>::quiet_NaN();
        xt::xarray<bool> isColAssigned = !xt::isnan(colSoln);
        xt::xarray<size_t> unassignedCols = xt::filter(rowIdx, !isColAssigned);

        assignment = xt::view(assignment, xt::keep(rowIdx));
        assignment = xt::view(assignment, xt::keep(xt::filter(rowIdx, isRowAssigned)));

        return std::make_tuple(assignment, unassignedRows, unassignedCols);
    }
}