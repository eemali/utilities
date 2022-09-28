#ifndef HELPER_BISTATIC_RANGE_FUSION_H
#define HELPER_BISTATIC_RANGE_FUSION_H
#include "../../include/objectDetection.h"
#include <tuple>
std::tuple<xt::xarray<double>, xt::xarray<double>>
helperBistaticRangeFusion(const std::vector<objectDetection> &detections,
                          bool isCovRequired = false);
#endif // !HELPER_BISTATIC_RANGE_FUSION_H
