#ifndef K_BEST_ENFORCE_CONSTRAINTS_H
#define K_BEST_ENFORCE_CONSTRAINTS_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    /**
     * @brief Enforce the constraints on a costMatrix.
     * 
     * This function enforces the constraints on a costMatrix. tuplesToRemove is
     * a N-by-2 list, which defines the assignments which should not be a part of
     * the assignment. tuplesToEnforce is a N-by-2 list, which defines tuples
     * which should be a part of the assignment. 
     */
    xt::xarray<double> kbestEnforceConstraints(const xt::xarray<double> &costMatrix,
                                               const xt::xarray<size_t> &tuplesToRemove,
                                               const xt::xarray<size_t> &tuplesToEnforce);
}
#endif // !K_BEST_ENFORCE_CONSTRAINTS_H
