#include "../include/assignjv.h"
#include "../internal/include/lapAuction.h"
#include "../internal/include/lapCheckCostMatrix.h"
#include "../internal/include/lapCheckUnassignedCost.h"
#include "../internal/include/lapColumnReduction.h"
#include "../internal/include/lapDijkstra.h"
#include "../internal/include/lapPadForUnassignedRowsAndColumns.h"
#include "../internal/include/lapReductionTransfer.h"
#include "../internal/include/lapRemoveImpossibles.h"
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    assignjv(xt::xarray<double> costMatrix,
             double costOfNonAssignment)
    {
        // check inputs
        lapCheckCostMatrix(costMatrix);
        lapCheckUnassignedCost(costOfNonAssignment);

        xt::xarray<size_t> rowIdx, colIdx, unassignedRows, unassignedColumns;
        std::tie(costMatrix, rowIdx, colIdx, unassignedRows, unassignedColumns) =
            lapRemoveImpossibles(costMatrix, costOfNonAssignment);

        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);
        costMatrix = lapPadForUnassignedRowsAndColumns(costMatrix, costOfNonAssignment);

        // Compute linear assignment
        xt::xarray<double> rowSoln, colSoln, colRedux;
        xt::xarray<size_t> rowAssignedCnt;
        std::tie(rowSoln, colSoln, colRedux, rowAssignedCnt) = lapColumnReduction(costMatrix);
        colRedux = lapReductionTransfer(costMatrix, rowSoln, colRedux, rowAssignedCnt);
        std::tie(rowSoln, colSoln, colRedux) = lapAuction(costMatrix, rowSoln, colSoln, colRedux, 2, false);
        std::tie(rowSoln, colSoln, colRedux) = lapDijkstra(costMatrix, rowSoln, colSoln, colRedux);
        // Remove assignments to padded rows and columns
        rowSoln = xt::view(rowSoln, xt::keep(xt::arange<size_t>(nRow)));
        colSoln = xt::view(colSoln, xt::keep(xt::arange<size_t>(nCol)));
        xt::filtration(rowSoln, rowSoln >= nCol) = std::numeric_limits<double>::quiet_NaN();
        xt::filtration(colSoln, colSoln >= nRow) = std::numeric_limits<double>::quiet_NaN();
        // Place assignments into the expected return format
        xt::xarray<bool> isRowAssigned = !xt::isnan(rowSoln);
        xt::xarray<bool> isColAssigned = !xt::isnan(colSoln);
        xt::xarray<size_t> assignments = xt::stack(xt::xtuple(xt::filter(rowIdx, isRowAssigned),
                                                              xt::view(colIdx, xt::keep(xt::filter(rowSoln, isRowAssigned)))),
                                                   1);
        unassignedRows = xt::concatenate(xt::xtuple(unassignedRows,
                                                    xt::filter(rowIdx, !isRowAssigned)),
                                         0);
        unassignedColumns = xt::concatenate(xt::xtuple(unassignedColumns,
                                                       xt::filter(colIdx, !isColAssigned)),
                                            0);
        return std::make_tuple(std::move(assignments),
                               std::move(unassignedRows),
                               std::move(unassignedColumns));
    }
}