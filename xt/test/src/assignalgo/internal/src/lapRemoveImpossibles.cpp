#include "../include/lapRemoveImpossibles.h"
#include <xtensor/xview.hpp>
namespace assignalgo
{
    std::tuple<xt::xarray<double>,
               xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    lapRemoveImpossibles(xt::xarray<double> costMatrix,
                         double costOfNonAssignment)
    {
        // Gate values in the cost matrix which are greater-than or equal-to twice
        // the cost of non-assignment
        double Inf = std::numeric_limits<double>::infinity();
        std::for_each(costMatrix.begin(), costMatrix.end(),
                      [&](double &v)
                      {
                          if (v >= 2 * costOfNonAssignment)
                          {
                              v = Inf;
                          }
                      });

        // Remove gated (set to Inf) rows or columns in the costMatrix where no
        // assignment is possibl
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        xt::xarray<size_t> rowIdx = xt::arange<size_t>(nRow);
        xt::xarray<size_t> colIdx = xt::arange<size_t>(nCol);

        xt::xarray<bool> isinf_costMatrix = xt::isinf(costMatrix);
        auto all_functor = xt::make_xreducer_functor([](bool a, bool b)
                                                     { return a & b; },
                                                     xt::const_value<bool>(true));
        xt::xarray<bool> isUnassignedRows = xt::reduce(all_functor, isinf_costMatrix, {1});
        xt::xarray<bool> isUnassignedColumns = xt::reduce(all_functor, isinf_costMatrix, {0});

        xt::xarray<size_t> unassignedRows = xt::flatten_indices(xt::argwhere(isUnassignedRows));
        xt::xarray<size_t> unassignedColumns = xt::flatten_indices(xt::argwhere(isUnassignedColumns));

        costMatrix = xt::view(costMatrix, xt::drop(unassignedRows), xt::drop(unassignedColumns));
        rowIdx = xt::view(rowIdx, xt::drop(unassignedRows));
        colIdx = xt::view(colIdx, xt::drop(unassignedColumns));

        return std::make_tuple(std::move(costMatrix),
                               std::move(rowIdx),
                               std::move(colIdx),
                               std::move(unassignedRows),
                               std::move(unassignedColumns));
    }
}