#ifndef COMPUTE_ASSOCIATION_BY_INDEX_H
#define COMPUTE_ASSOCIATION_BY_INDEX_H
#include "objectDetection.h"
#include <tuple>
typedef std::tuple<xt::xarray<double>,
                   xt::xarray<double>> (*stateEstimationFcn_t)(const std::vector<objectDetection> &detections,
                                                               bool isCovRequired);

typedef xt::xarray<double> (*measurementFcn_t)(const xt::xarray<double> &pos,
                                               const std::vector<MeasurementParameters_t> &measurementParameters);

std::tuple<xt::xarray<double>, double>
computeAssociationByIndex(const xt::xarray<double> &fusedStates,
                          const xt::xarray<double> &costValues,
                          const std::vector<std::vector<objectDetection>> &pDetections,
                          stateEstimationFcn_t stateEstimationFcn,
                          measurementFcn_t measurementFcn,
                          size_t minValidTriangulation,
                          const std::vector<size_t> &sizeValues,
                          size_t numSensors,
                          const xt::xarray<double> &Pd,
                          const xt::xarray<double> &Pfa,
                          const xt::xarray<double> &V,
                          size_t i);
#endif // !COMPUTE_ASSOCIATION_BY_INDEX_H
