#ifndef CALC_ND_COST_MATRIX_H
#define CALC_ND_COST_MATRIX_H
#include "computeAssociationByIndex.h"
std::tuple<xt::xarray<double>, xt::xarray<double>>
calcNDCostMatrix(const xt::xarray<double> &fusedStates,
                 const xt::xarray<double> &costValues,
                 const std::vector<std::vector<objectDetection>> &pDetections,
                 stateEstimationFcn_t stateEstimationFcn,
                 measurementFcn_t measurementFcn,
                 size_t minValidTriangulation,
                 const std::vector<size_t> &sizeValues,
                 const xt::xarray<double> &Pd,
                 const xt::xarray<double> &Pfa,
                 const xt::xarray<double> &V);
#endif // !CALC_ND_COST_MATRIX_H
