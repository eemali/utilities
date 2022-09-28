#include "../include/helperBistaticRangeFusion.h"
#include "../include/helperSphericalIntersection.h"
#include "../../include/cvmeasjac.h"
#include <xtensor/xnorm.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor-blas/xlinalg.hpp>
using namespace xt::placeholders;
// Supporting Functions
// --------------------
// linear fusion function for measurement noise
static xt::xarray<double> linearFusionFcn(const xt::xarray<double> &pos,
                                          const std::vector<objectDetection> &thisDetections);

// This function returns the estimated position and covariance of the target
// given the bistatic detections generated from it.
std::tuple<xt::xarray<double>, xt::xarray<double>>
helperBistaticRangeFusion(const std::vector<objectDetection> &detections,
                          bool isCovRequired)
{
    xt::xarray<double> pos, cov;
    // Do a coarse gating, as a minimum of 3 measurements are required for
    // finding a solution.
    size_t num_detections = detections.size();
    if (num_detections < 3)
    {
        pos = xt::xarray<double>({3}, 1e10);
        cov = 2 * xt::eye<double>(3);
    }
    else
    {
        // Retrieve info from measurements
        xt::xarray<double> ranges = xt::zeros<double>({num_detections});
        xt::xarray<double> receiverLocations = xt::zeros<double>({size_t(3), num_detections});
        xt::xarray<double> emitterLocation = detections.at(0).MeasurementParameters.at(0).EmitterPosition;

        xt::xarray<double> rLoc;
        double L;
        for (size_t i = 0; i < num_detections; i++)
        {
            rLoc = detections.at(i).MeasurementParameters.at(1).OriginPosition;
            xt::col(receiverLocations, i) = rLoc;

            // The spherical intersection method assumes that measurment is
            // Remit + Rrecv. Bistatic measurement is defined as Remit + Rrecv - Rb.
            // Add the Rb to the actual measurement
            L = xt::norm_l2(emitterLocation - rLoc)();
            ranges.at(i) = detections.at(i).Measurement.at(0) + L;
        }
        pos = helperSphericalIntersection(ranges, receiverLocations, emitterLocation);

        // Covariance is calculated only when required. This helps saving
        // computation during cost calculation for static fusion, where only
        // position is required.
        if (isCovRequired)
        {
            cov = linearFusionFcn(pos, detections);
        }
    }
    return std::make_tuple(pos, cov);
}
// linear fusion function for measurement noise
xt::xarray<double> linearFusionFcn(const xt::xarray<double> &pos,
                                   const std::vector<objectDetection> &thisDetections)
{
    // Linear noise fusion function. It requires measJacobian to use linear
    // transformation.
    // Use a constant velocity state to calculate jacobians.
    xt::xarray<double> estState = xt::zeros<double>({6});
    xt::view(estState, xt::range(_, _, 2)) = pos;
    size_t n = thisDetections.size();
    xt::xarray<double> totalJacobian = xt::zeros<double>({n, size_t(3)});
    xt::xarray<double> totalCovariance = xt::zeros<double>({n, n});
    xt::xarray<double> H;
    for (size_t i = 0; i < n; i++)
    {
        H = cvmeasjac(estState, thisDetections.at(i).MeasurementParameters);
        xt::row(totalJacobian, i) = xt::view(H, 1, xt::range(_, _, 2));
        totalCovariance.at(i, i) = thisDetections.at(i).MeasurementNoise.at(0);
    }
    xt::xarray<double> toInvertJacobian = xt::linalg::dot(xt::transpose(totalJacobian), xt::linalg::dot(xt::linalg::pinv(totalCovariance), totalJacobian)); // std::get<0>(xt::linalg::lstsq(totalCovariance, totalJacobian))
    xt::xarray<double> I = xt::eye<double>(3);
    // 2-D to 3-D conversion with 0 jacobian wrt z.
    if (0 == toInvertJacobian.at(2, 2))
    {
        toInvertJacobian.at(2, 2) = 1;
    }
    xt::xarray<double> measCov = xt::linalg::pinv(toInvertJacobian); // std::get<0>(xt::linalg::lstsq(toInvertJacobian, I));
    // Return true positive definite.
    measCov = 0.5 * (measCov + xt::transpose(measCov));
    xt::filtration(measCov, !xt::isfinite(measCov)) = 1000; // Some big number for inf and nan
    return measCov;
}