#include "../include/lapRemoveImpossibles.h"
#include <xtensor/xview.hpp>
namespace assignalgo
{
    xt::xarray<double> lapRemoveImpossibles(xt::xarray<double> costMatrix,
                                            double costOfNonAssignment,
                                            xt::xarray<size_t> &rowIdx,
                                            xt::xarray<size_t> &colIdx,
                                            xt::xarray<size_t> &unassignedRows,
                                            xt::xarray<size_t> &unassignedColumns)
    {
        // Gate values in the cost matrix which are greater-than or equal-to twice
        // the cost of non-assignment
        double inf = std::numeric_limits<double>::infinity();
        std::for_each(costMatrix.begin(), costMatrix.end(),
                      [&](double &v) {if (v >= 2 * costOfNonAssignment){v = inf;} });

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

        return costMatrix;
    }
}