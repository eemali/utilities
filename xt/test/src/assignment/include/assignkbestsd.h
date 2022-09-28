#ifndef ASSIGN_K_BEST_SD_H
#define ASSIGN_K_BEST_SD_H
#include <xtensor/xarray.hpp>
#include "../include/assignauction.h"
#include <vector>
namespace assignalgo
{
    typedef void (*algFcn_t)(xt::xarray<double>,
                             double,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &);

    void assignkbestsd(xt::xarray<double> costMatrix,
                       std::vector<xt::xarray<size_t>> &assignments,
                       std::vector<double> &cost,
                       std::vector<double> &gap,
                       size_t numSolutions = 1,
                       double desiredGap = 0.01,
                       size_t maxIterations = 100,
                       algFcn_t algFcn = &assignauction);
}
#endif // !ASSIGN_K_BEST_SD_H
