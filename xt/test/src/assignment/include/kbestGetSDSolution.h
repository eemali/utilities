#ifndef K_BEST_GET_SD_SOLUTION_H
#define K_BEST_GET_SD_SOLUTION_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    typedef void (*algFcn_t)(xt::xarray<double>,
                             double,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &);

    void kbestGetSDSolution(xt::xarray<double> costMatrix,
                            double desiredGap,
                            size_t maxIterations,
                            algFcn_t algFcn,
                            xt::xarray<size_t> &soln,
                            double *cost = nullptr,
                            double *gap = nullptr,
                            bool *isValid = nullptr);
}
#endif // !K_BEST_GET_SD_SOLUTION_H
