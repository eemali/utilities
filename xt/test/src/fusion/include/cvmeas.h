#ifndef CV_MEAS_H
#define CV_MEAS_H
#include "objectDetection.h"
xt::xarray<double> cvmeas(const xt::xarray<double> &state,
                          const std::vector<MeasurementParameters_t> &MeasurementParameters);
#endif // !CV_MEAS_H
