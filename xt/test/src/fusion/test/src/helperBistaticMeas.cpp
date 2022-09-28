#include "../include/helperBistaticMeas.h"
#include "../../include/cvmeas.h"
#include <xtensor/xnorm.hpp>
#include <xtensor/xview.hpp>
#include <xtensor-blas/xlinalg.hpp>
using namespace xt::placeholders;
xt::xarray<double> helperBistaticMeas(const xt::xarray<double> &pos,
                                      const std::vector<MeasurementParameters_t> &measurementParameters)
{
    // We can use a constant velocity state to calculate the ranges.
    xt::xarray<double> state = xt::zeros<double>({6});
    xt::view(state, xt::range(_, _, 2)) = pos;

    // Target to receiver range
    // The origin position and orientation etc. is set in the
    // MeasurementParameters for the target. The output will be target
    // range.
    xt::xarray<double> Rrec = cvmeas(state, measurementParameters);

    // Get measurement range for the emitter.
    xt::xarray<double> emitterPosition = measurementParameters.at(0).EmitterPosition;
    double Remit = xt::norm_l2(pos - emitterPosition)();

    // Distance between emitter and receiver
    xt::xarray<double> rxGlobal = measurementParameters.at(1).OriginPosition + xt::linalg::dot(measurementParameters.at(1).Orientation, measurementParameters.at(0).OriginPosition);
    double Rb = xt::norm_l2(rxGlobal - emitterPosition)();

    // measurement
    xt::xarray<double> meas = Rrec + Remit - Rb;

    return meas;
}