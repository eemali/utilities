#ifndef LAP_REMOVE_IMPOSSIBLES_H
#define LAP_REMOVE_IMPOSSIBLES_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    xt::xarray<double> lapRemoveImpossibles(xt::xarray<double> costMatrix,
                                            double costOfNonAssignment,
                                            xt::xarray<size_t> &rowIdx,
                                            xt::xarray<size_t> &colIdx,
                                            xt::xarray<size_t> &unassignedRows,
                                            xt::xarray<size_t> &unassignedColumns);
}
#endif // !LAP_REMOVE_IMPOSSIBLES_H