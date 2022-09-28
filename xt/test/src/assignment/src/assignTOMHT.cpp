#include "../include/assignTOMHT.h"
#include <xtensor/xadapt.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
namespace assignalgo
{
    static bool isempty(const xt::xarray<size_t> &arr);

    void assignTOMHT(xt::xarray<double> &costMatrix,
                     xt::xarray<double> &costThreshold,
                     xt::xarray<size_t> &assignments,
                     xt::xarray<size_t> &unassignedTracks,
                     xt::xarray<size_t> &unassignedDetections)
    {
        // Validate inputs
        assert(2 == costMatrix.shape().size() && !xt::any(xt::isnan(costMatrix)));
        // costThreshold must be a 3-element finite nondecreasing vector
        assert(3 == costThreshold.size() &&
               xt::all(xt::isfinite(costThreshold)) &&
               costThreshold.at(0) <= costThreshold.at(1) &&
               costThreshold.at(1) <= costThreshold.at(2));

        // Allocate memory
        size_t numTracks = costMatrix.shape(0);
        size_t numDetections = costMatrix.shape(1);
        unassignedTracks = xt::arange<size_t>(numTracks);
        unassignedDetections = xt::arange<size_t>(numDetections);

        // Calculate all the possible assignments in C3Gate
        xt::xarray<size_t> idx = xt::from_indices(xt::argwhere(costMatrix < costThreshold.at(2)));
        xt::xarray<size_t> rows = xt::col(idx, 0);
        xt::xarray<size_t> columns = xt::col(idx, 1);

        if (!isempty(rows))
        {
            assignments = xt::stack(xt::xtuple(rows, columns), 1);
        }
        else
        {
            assignments = xt::xarray<size_t>::from_shape({0, 2});
        }

        // Find all tracks that have at least one detection in C1Gate
        auto any_functor = xt::make_xreducer_functor([](bool a, bool b) { return a | b; }, xt::const_value<bool>(false));
        xt::xarray<bool> r = !xt::reduce(any_functor, costMatrix < costThreshold.at(0), {1});
        unassignedTracks = xt::filter(unassignedTracks, r);

        // Find all detections that have at least one track in C2Gate
        xt::xarray<bool> t = !xt::reduce(any_functor, costMatrix < costThreshold.at(1), {0});
        unassignedDetections = xt::filter(unassignedDetections, t);
    }

    static bool isempty(const xt::xarray<size_t> &arr)
    {
        return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
    }
}