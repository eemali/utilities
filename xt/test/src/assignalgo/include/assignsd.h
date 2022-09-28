#ifndef ASSIGN_SD_H
#define ASSIGN_SD_H
#include "assignauction.h"
#include "../internal/include/assigntypedef.h"
namespace assignalgo
{
    std::tuple<xt::xarray<size_t>, double, double>
    assignsd(xt::xarray<double> costMatrix,
             double desiredGap = 0.01,
             size_t maxIterations = 100,
             assign2dFcn_t algFcn = &assignauction);
}
#endif // !ASSIGN_SD_H