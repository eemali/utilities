#ifndef HELPER_BISTATIC_MEAS_H
#define HELPER_BISTATIC_MEAS_H
#include "../../include/objectDetection.h"
xt::xarray<double> helperBistaticMeas(const xt::xarray<double> &pos,
                                      const std::vector<MeasurementParameters_t> &measurementParameters);
#endif // !HELPER_BISTATIC_MEAS_H
