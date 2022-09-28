#include "../include/calcNDCostMatrix.h"
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
std::tuple<xt::xarray<double>, xt::xarray<double>>
calcNDCostMatrix(const xt::xarray<double> &fusedStates,
                 const xt::xarray<double> &costValues,
                 const std::vector<std::vector<objectDetection>> &pDetections,
                 stateEstimationFcn_t stateEstimationFcn,
                 measurementFcn_t measurementFcn,
                 size_t minValidTriangulation,
                 const std::vector<size_t> &sizeValues,
                 const xt::xarray<double> &Pd,
                 const xt::xarray<double> &Pfa,
                 const xt::xarray<double> &V)
{
    size_t numAssociations = fusedStates.shape(1);
    size_t numSensors = pDetections.size();

    xt::xarray<double> fusedStatesOut = xt::zeros<double>(fusedStates.shape());
    xt::xarray<double> costValuesOut = xt::zeros<double>(costValues.shape());

    xt::xarray<double> fusedStatesOuti;
    double costValuesOuti;
    for (size_t i = 0; i < numAssociations; i++)
    {
        std::tie(fusedStatesOuti, costValuesOuti) = computeAssociationByIndex(fusedStates, costValues, pDetections,
                                                                              stateEstimationFcn, measurementFcn,
                                                                              minValidTriangulation, sizeValues,
                                                                              numSensors, Pd, Pfa, V, i);

        xt::col(fusedStatesOut, i) = fusedStatesOuti;
        costValuesOut.at(i) = costValuesOuti;
    }

    // Fine-gating of scores. We can set it to inf to permanently gate them.
    // Note that one must not set the cost of false alarms/missed detections as
    // inf. They are set to 0 in this case.
    xt::filtration(costValuesOut, costValuesOut > 0) = std::numeric_limits<double>::infinity();
    // Assemble cost values in a cost matrix.
    xt::xarray<double> costMatrix = xt::reshape_view(costValuesOut, sizeValues);
    return std::make_tuple(fusedStatesOut, costMatrix);
}