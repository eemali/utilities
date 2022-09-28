#include "../include/cvmeasjac.h"
#include "../include/global2localcoordjac.h"
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
xt::xarray<double> cvmeasjac(const xt::xarray<double> &state,
                             const std::vector<MeasurementParameters_t> &MeasurementParameters)
{
    size_t numStates = state.size();
    if ((2 != numStates) && (4 != numStates) && (6 != numStates))
    {
        XTENSOR_THROW(std::runtime_error, "incorrectStateVecWithInfo [2, 4, 6]");
    }
    bool isRect = Frame_t::RECTANGULAR == MeasurementParameters.at(0).Frame;
    xt::xarray<double> sensorpos = MeasurementParameters.at(1).OriginPosition;
    xt::xarray<double> sensorvel = MeasurementParameters.at(0).OriginVelocity;
    xt::xarray<double> localAxes = MeasurementParameters.at(0).Orientation;
    bool hasAz = MeasurementParameters.at(0).HasAzimuth;
    bool hasEl = MeasurementParameters.at(0).HasElevation;
    bool hasVel = MeasurementParameters.at(0).HasVelocity;
    bool hasRange = MeasurementParameters.at(0).HasRange;

    // Calculate the Jacobian
    xt::xarray<double> jacobian, A;
    if (isRect)
    {
        if (!hasVel)
        {
            jacobian = xt::zeros<double>({size_t(3), numStates});
            // Essentially, eye(3) * localAxes', where eye(3) is the Jacobian of
            // measurement to [x;y;z]
            A = xt::transpose(localAxes);
            xt::view(jacobian, xt::all(), xt::range(_, _, 2)) = A;
        }
        else
        {
            jacobian = xt::zeros<double>({size_t(6), numStates});
            // Essentially, eye(3) * localAxes', where eye(3) is the Jacobian of
            // measurement to both[x; y; z] and [vx; vy; vz]
            A = xt::transpose(localAxes);
            xt::view(jacobian, xt::range(_, 3), xt::range(_, _, 2)) = A;
            xt::view(jacobian, xt::range(3, _), xt::range(1, _, 2)) = A;
        }
    }
    else // Spherical coordinate system.
    {
        // Range-rate available if hasRange is true
        bool hasRangeRate = hasVel && hasRange;

        // Define the size of the measurement
        size_t measSize = hasAz + hasEl + hasRange + hasRangeRate;

        jacobian = xt::zeros<double>({measSize, numStates}); // Up to 4 outputs (az, el, r, rr) by n states

        xt::xarray<double> tgtpos = xt::zeros<double>({3});
        xt::view(tgtpos, xt::range(_, numStates / 2)) = xt::view(state, xt::range(_, numStates - 1, 2));

        A = global2localcoordjac(tgtpos, sensorpos, localAxes);

        xt::xarray<bool> measLogicalIndex({hasAz, hasEl, hasRange});
        if (!hasRangeRate)
        {
            xt::view(jacobian, xt::all(), xt::range(_, _, 2)) = xt::view(A,
                                                                         xt::keep(xt::from_indices(xt::argwhere(measLogicalIndex))),
                                                                         xt::range(_, numStates / 2));
        }
        else
        {
            XTENSOR_THROW(std::runtime_error, "To be implemented");
        }
    }
    return jacobian;
}