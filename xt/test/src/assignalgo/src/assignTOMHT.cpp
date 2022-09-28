#include "../include/assignTOMHT.h"
#include "../internal/include/assigntypedef.h"
#include <xtensor/xadapt.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
namespace assignalgo
{
    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    assignTOMHT(const xt::xarray<double> &costMatrix,
                const xt::xarray<double> &costThreshold)
    {
        // Validate inputs
        assert(2 == costMatrix.dimension() && !xt::any(xt::isnan(costMatrix)));
        // costThreshold must be a 3-element finite nondecreasing vector
        assert(3 == costThreshold.size() &&
               xt::all(xt::isfinite(costThreshold)) &&
               costThreshold.at(0) <= costThreshold.at(1) &&
               costThreshold.at(1) <= costThreshold.at(2));

        // Allocate memory
        size_t numTracks = costMatrix.shape(0);
        size_t numDetections = costMatrix.shape(1);
        xt::xarray<size_t> unassignedTracks = xt::arange<size_t>(numTracks);
        xt::xarray<size_t> unassignedDetections = xt::arange<size_t>(numDetections);

        // Calculate all the possible assignments in C3Gate
        xt::xarray<size_t> idx = xt::from_indices(xt::argwhere(costMatrix < costThreshold.at(2)));
        xt::xarray<size_t> rows = xt::col(idx, 0);
        xt::xarray<size_t> columns = xt::col(idx, 1);

        xt::xarray<size_t> assignments;
        if (!isempty(rows))
        {
            assignments = xt::stack(xt::xtuple(rows, columns), 1);
        }
        else
        {
            assignments = xt::xarray<size_t>::from_shape({0, 2});
        }

        // Find all tracks that have at least one detection in C1Gate
        auto any_functor = xt::make_xreducer_functor([](bool a, bool b)
                                                     { return a | b; },
                                                     xt::const_value<bool>(false));
        xt::xarray<bool> r = !xt::reduce(any_functor, costMatrix < costThreshold.at(0), {1});
        unassignedTracks = xt::filter(unassignedTracks, r);

        // Find all detections that have at least one track in C2Gate
        xt::xarray<bool> t = !xt::reduce(any_functor, costMatrix < costThreshold.at(1), {0});
        unassignedDetections = xt::filter(unassignedDetections, t);

        return std::make_tuple(std::move(assignments),
                               std::move(unassignedTracks),
                               std::move(unassignedDetections));
    }
}