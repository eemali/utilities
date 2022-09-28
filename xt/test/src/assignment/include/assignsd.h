#ifndef ASSIGN_SD_H
#define ASSIGN_SD_H
#include <xtensor/xarray.hpp>
#include "../include/assignauction.h"
namespace assignalgo
{
    typedef void (*algFcn_t)(xt::xarray<double>,
                             double,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &);
    void assignsd(xt::xarray<double> costMatrix,
                  xt::xarray<size_t> &assignments,
                  double *costOfAssignment = nullptr,
                  double *solutionGap = nullptr,
                  double desiredGap = 0.01,
                  size_t maxIterations = 100,
                  algFcn_t algFcn = &assignauction);
}
#endif // !ASSIGN_SD_H