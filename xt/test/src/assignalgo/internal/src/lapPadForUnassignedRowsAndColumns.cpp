#include "../include/lapPadForUnassignedRowsAndColumns.h"
namespace assignalgo
{
    xt::xarray<double> lapPadForUnassignedRowsAndColumns(xt::xarray<double> costMatrix,
                                                         double costOfNonAssignment)
    {
        // Pad matrix with dummy rows and columns to include the costOfNonAssignment
        // in the solution
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        double Inf = std::numeric_limits<double>::infinity();
        xt::xarray<double> dummyCols = xt::xarray<double>({nRow, nRow}, Inf);
        for (size_t idx : xt::arange<size_t>(nRow))
        {
            dummyCols.at(idx, idx) = costOfNonAssignment;
        }
        xt::xarray<double> dummyRows = xt::xarray<double>({nCol, nCol}, Inf);
        for (size_t idx : xt::arange<size_t>(nCol))
        {
            dummyRows.at(idx, idx) = costOfNonAssignment;
        }
        costMatrix = xt::vstack(xt::xtuple(xt::hstack(xt::xtuple(costMatrix, dummyCols)),
                                           xt::hstack(xt::xtuple(dummyRows, xt::zeros<double>({nCol, nRow})))));
        return costMatrix;
    }
}