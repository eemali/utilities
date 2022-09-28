#ifndef LAP_PAD_FOR_UNASSIGNED_ROWS_AND_COLUMNS_H
#define LAP_PAD_FOR_UNASSIGNED_ROWS_AND_COLUMNS_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    xt::xarray<double> lapPadForUnassignedRowsAndColumns(xt::xarray<double> costMatrix,
                                                         double costOfNonAssignment);
}
#endif // !LAP_PAD_FOR_UNASSIGNED_ROWS_AND_COLUMNS_H