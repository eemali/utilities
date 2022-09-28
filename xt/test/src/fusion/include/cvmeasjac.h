#ifndef CV_MEAS_JAC_H
#define CV_MEAS_JAC_H
#include "objectDetection.h"
xt::xarray<double> cvmeasjac(const xt::xarray<double> &state,
                             const std::vector<MeasurementParameters_t> &MeasurementParameters);
#endif // !CV_MEAS_JAC_H
