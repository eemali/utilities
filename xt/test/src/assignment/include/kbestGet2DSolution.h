#ifndef K_BEST_GET_2D_SOLUTION_H
#define K_BEST_GET_2D_SOLUTION_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    typedef void (*algFcn_t)(xt::xarray<double>,
                             double,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &);

    void kbestGet2DSolution(xt::xarray<double> costMatrix,
                            double costOfNonAssignment,
                            algFcn_t algFcn,
                            xt::xarray<size_t> &solution,
                            double *cost = nullptr,
                            bool *isValidSoln = nullptr);
}
#endif // !K_BEST_GET_2D_SOLUTION_H
