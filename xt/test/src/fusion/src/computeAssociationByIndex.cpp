#include "../include/computeAssociationByIndex.h"
#include <xtensor/xview.hpp>
// Supporting Functions
// --------------------
double gaussLikelihood(const xt::xarray<double> &res,
                       const xt::xarray<double> &S);

std::tuple<std::vector<objectDetection>,
           xt::xarray<bool>>
parseBuffer(size_t numSensors,
            const std::vector<std::vector<objectDetection>> &pDetections,
            const std::vector<size_t> &sizeValues,
            size_t i);

std::tuple<xt::xarray<double>, double>
computeStateAndCost(size_t numSensors,
                    const std::vector<objectDetection> &thisBuffer,
                    const xt::xarray<bool> &detIndicators,
                    stateEstimationFcn_t stateEstimationFcn,
                    measurementFcn_t measurementFcn,
                    const xt::xarray<double> &Pd,
                    const xt::xarray<double> &Pfa,
                    const xt::xarray<double> &V);

std::tuple<xt::xarray<double>, double>
computeAssociationByIndex(const xt::xarray<double> &fusedStates,
                          const xt::xarray<double> &costValues,
                          const std::vector<std::vector<objectDetection>> &pDetections,
                          stateEstimationFcn_t stateEstimationFcn,
                          measurementFcn_t measurementFcn,
                          size_t minValidTriangulation,
                          const std::vector<size_t> &sizeValues,
                          size_t numSensors,
                          const xt::xarray<double> &Pd,
                          const xt::xarray<double> &Pfa,
                          const xt::xarray<double> &V,
                          size_t i)
{
    std::vector<objectDetection> thisBuffer;
    xt::xarray<bool> detIndicators;
    std::tie(thisBuffer, detIndicators) = parseBuffer(numSensors, pDetections, sizeValues, i);

    xt::xarray<double> statesOut;
    double costOut;
    // Coarse-gating - minimum 2 detections required in the tuple.
    if (thisBuffer.size() >= minValidTriangulation)
    {
        std::tie(statesOut, costOut) = computeStateAndCost(numSensors, thisBuffer, detIndicators,
                                                           stateEstimationFcn, measurementFcn, Pd, Pfa, V);
    }
    else
    {
        statesOut = xt::col(fusedStates, i);
        costOut = costValues.at(i);
    }
    return std::make_tuple(statesOut, costOut);
}
double gaussLikelihood(const xt::xarray<double> &res,
                       const xt::xarray<double> &S)
{
    double t1 = std::log(2 * xt::numeric_constants<double>::PI * S.at(0));
    double t2 = res.at(0) / S.at(0) * res.at(0);
    double logLikelihood = t1 + t2;
    // std::log(xt::linalg::det(2 * xt::numeric_constants<double>::PI * S)) +
    //                        xt::linalg::dot(res, xt::linalg::dot(xt::linalg::inv(S), res))();
    return logLikelihood;
}

std::tuple<std::vector<objectDetection>,
           xt::xarray<bool>>
parseBuffer(size_t numSensors,
            const std::vector<std::vector<objectDetection>> &pDetections,
            const std::vector<size_t> &sizeValues,
            size_t i)
{
    auto thisIndices = xt::unravel_index(i, sizeValues);
    xt::xarray<bool> detIndicators = xt::xarray<bool>({numSensors}, false);

    std::vector<objectDetection> thisBuffer;
    for (size_t k = 0; k < numSensors; k++)
    {
        if (thisIndices.at(k) > 0)
        {
            thisBuffer.push_back(pDetections.at(k).at(thisIndices.at(k) - 1));
            detIndicators.at(k) = true;
        }
    }
    return std::make_tuple(thisBuffer, detIndicators);
}

std::tuple<xt::xarray<double>, double>
computeStateAndCost(size_t numSensors,
                    const std::vector<objectDetection> &thisBuffer,
                    const xt::xarray<bool> &detIndicators,
                    stateEstimationFcn_t stateEstimationFcn,
                    measurementFcn_t measurementFcn,
                    const xt::xarray<double> &Pd,
                    const xt::xarray<double> &Pfa,
                    const xt::xarray<double> &V)
{
    xt::xarray<double> state;
    std::tie(state, std::ignore) = stateEstimationFcn(thisBuffer, false);
    xt::xarray<double> indLogLikelihood = xt::zeros<double>({numSensors});
    double negCost = xt::sum((detIndicators - 1) * xt::log(1 - Pd) - detIndicators * xt::log(Pd * V / Pfa))();
    double thisCost;
    if (negCost < 0)
    {
        xt::xarray<double> zEst, zAct, r, S;
        for (size_t m = 0; m < thisBuffer.size(); m++)
        {
            zEst = measurementFcn(state, thisBuffer.at(m).MeasurementParameters);
            zAct = thisBuffer.at(m).Measurement;
            r = zEst - zAct;
            S = thisBuffer.at(m).MeasurementNoise;
            indLogLikelihood.at(m) = gaussLikelihood(r, S);
        }
    }
    thisCost = negCost + xt::sum(indLogLikelihood)();
    return std::make_tuple(state, thisCost);
}