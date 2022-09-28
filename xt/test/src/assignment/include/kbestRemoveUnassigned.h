#ifndef K_BEST_REMOVE_UNASSIGNED_H
#define K_BEST_REMOVE_UNASSIGNED_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    void kbestRemoveUnassigned(xt::xarray<size_t> &assignment,
                               const xt::xarray<size_t> &costSize,
                               xt::xarray<size_t> *unassignedRows = nullptr,
                               xt::xarray<size_t> *unassignedCols = nullptr);
}
#endif // !K_BEST_REMOVE_UNASSIGNED_H
