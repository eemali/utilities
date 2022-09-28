#ifndef K_BEST_ENFORCE_CONSTRAINTS_H
#define K_BEST_ENFORCE_CONSTRAINTS_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    xt::xarray<double> kbestEnforceConstraints(const xt::xarray<double> &costMatrix,
                                               const xt::xarray<size_t> &tuplesToRemove,
                                               const xt::xarray<size_t> &tuplesToEnforce);
}
#endif // !K_BEST_ENFORCE_CONSTRAINTS_H
