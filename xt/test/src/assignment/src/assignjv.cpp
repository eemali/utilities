#include "../include/assignjv.h"
#include "../include/lapAuction.h"
#include "../include/lapCheckCostMatrix.h"
#include "../include/lapCheckUnassignedCost.h"
#include "../include/lapColumnReduction.h"
#include "../include/lapDijkstra.h"
#include "../include/lapReductionTransfer.h"
#include "../include/lapRemoveImpossibles.h"
#include "../include/lapPadForUnassignedRowsAndColumns.h"
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    void assignjv(xt::xarray<double> costMatrix,
                  double costOfNonAssignment,
                  xt::xarray<size_t> &assignments,
                  xt::xarray<size_t> &unassignedRows,
                  xt::xarray<size_t> &unassignedColumns)
    {
        lapCheckCostMatrix(costMatrix);
        lapCheckUnassignedCost(costOfNonAssignment);
        xt::xarray<size_t> rowIdx, colIdx;
        costMatrix = lapRemoveImpossibles(costMatrix, costOfNonAssignment, rowIdx, colIdx, unassignedRows, unassignedColumns);
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);
        costMatrix = lapPadForUnassignedRowsAndColumns(costMatrix, costOfNonAssignment);
        xt::xarray<double> rowSoln, colSoln, colRedux;
        xt::xarray<size_t> rowAssignedCnt;
        lapColumnReduction(costMatrix, rowSoln, colSoln, colRedux, rowAssignedCnt);
        lapReductionTransfer(costMatrix, rowSoln, colRedux, rowAssignedCnt);
        lapAuction(costMatrix, rowSoln, colSoln, colRedux, 2, false);
        lapDijkstra(costMatrix, rowSoln, colSoln, colRedux);
        // Remove assignments to padded rows and columns
        rowSoln = xt::view(rowSoln, xt::keep(xt::arange<size_t>(nRow)));
        colSoln = xt::view(colSoln, xt::keep(xt::arange<size_t>(nCol)));
        xt::filtration(rowSoln, rowSoln >= nCol) = std::numeric_limits<double>::quiet_NaN();
        xt::filtration(colSoln, colSoln >= nRow) = std::numeric_limits<double>::quiet_NaN();
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