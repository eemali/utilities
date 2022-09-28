#include "../include/cvmeas.h"
#include "../include/global2localcoord.h"
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
xt::xarray<double> cvmeas(const xt::xarray<double> &state,
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

    // Calculate measurement
    xt::xarray<double> tgtpos = xt::zeros<double>({3});
    xt::xarray<double> tgtvel = xt::zeros<double>({3});
    xt::view(tgtpos, xt::range(_, numStates / 2)) = xt::view(state, xt::range(_, numStates - 1, 2));
    xt::view(tgtvel, xt::range(_, numStates / 2)) = xt::view(state, xt::range(1, numStates, 2));

    xt::xarray<double> measurement;
    if (isRect)
    {
        if (!hasVel)
        {
            measurement = global2localcoord(tgtpos, "rr", sensorpos, localAxes);
        }
        else
        {
            xt::xarray<double> posMeas = global2localcoord(tgtpos, "rr", sensorpos, localAxes);
            xt::xarray<double> velMeas = global2localcoord(tgtvel, "rr", sensorpos, localAxes);
            measurement = xt::vstack(xt::xtuple(posMeas, velMeas));
        }
    }
    else // Spherical coordinate system.
    {
        // Range-rate available if hasRange is true
        bool hasRangeRate = hasVel && hasRange;

        // Define the size of the measurement
        size_t measSize = hasAz + hasEl + hasRange + hasRangeRate;

        measurement = xt::zeros<double>({measSize}); // Up to 4 outputs (az, el, r, rr) by n states

        xt::xarray<double> meas = global2localcoord(tgtpos, "rs", sensorpos, localAxes);

        xt::xarray<bool> measLogicalIndex({hasAz, hasEl, hasRange});
        if (!hasRangeRate)
        {
            measurement = xt::view(meas,
                                   xt::keep(xt::from_indices(xt::argwhere(measLogicalIndex))));
        }
        else
        {
            XTENSOR_THROW(std::runtime_error, "To be implemented");
        }
    }
    return measurement;
}