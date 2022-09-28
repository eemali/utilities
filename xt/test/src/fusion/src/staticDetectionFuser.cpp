#include "../include/staticDetectionFuser.h"
#include "../include/calcNDCostMatrix.h"
#include "../../assignalgo/include/assignsd.h"
#include "../../assignalgo/include/assignjv.h"
#include "../../assignalgo/internal/include/assign2DRelaxed.h"
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
using namespace xt::placeholders;
staticDetectionFuser::staticDetectionFuser(stateEstimationFcn_t stateEstimationFcn,
                                           measurementFcn_t measurementFcn,
                                           size_t MaxNumSensors,
                                           size_t FusedSensorIndex,
                                           double DetectionProbability,
                                           double FalseAlarmRate,
                                           double Volume,
                                           double TimeTolerance) : pMeasurementFusionFcn(stateEstimationFcn),
                                                                   pMeasurementFcn(measurementFcn),
                                                                   pMaxNumSensors(MaxNumSensors),
                                                                   pFusedSensorIndex(FusedSensorIndex),
                                                                   pDetectionProbability(DetectionProbability),
                                                                   pFalseAlarmRate(FalseAlarmRate),
                                                                   pVolume(Volume),
                                                                   pTimeTolerance(TimeTolerance),
                                                                   pIsInitialized(false),
																   pDetectionTime(0),
                                                                   p2DAssignmentFcn([](xt::xarray<double> costMatrix)
                                                                                    { return std::get<0>(assignalgo::assign2DRelaxed(costMatrix, &assignalgo::assignjv)); }),
                                                                   pSDAssignmentFcn([](xt::xarray<double> costMatrix)
                                                                                    { return std::get<0>(assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignauction)); })
{
}
std::tuple<std::vector<objectDetection>,
           info_t>
staticDetectionFuser::fuse(const std::vector<objectDetection> &detections)
{
    if (!pIsInitialized)
    {
        fuser_initilization(detections);
        pIsInitialized = true;
    }
    // Get a list of detections separated by source.
    std::vector<std::vector<objectDetection>> pDetections;
    std::vector<std::vector<size_t>> globalIDs;
    std::tie(pDetections, globalIDs) = sortDetectionsBySource(detections);

    // Calculate cost matrix
    xt::xarray<double> costMatrix = calcCost(pDetections);

    // Calculate assignments
    xt::xarray<size_t> assignments, falseAlarmIDs;
	xt::xarray<int> globalAssignments;
    std::tie(assignments, globalAssignments, falseAlarmIDs) = calcAssignments(costMatrix, globalIDs);

    // Fuse measurement and measurement noise
    xt::xarray<double> meas, measCov;
    std::vector<std::vector<ObjectAttribute_t>> attribs;
    std::tie(meas, measCov, attribs) = fuseAssignedDetections(assignments, pDetections);

    // Assemble composite detections
    std::vector<objectDetection> compositeDetections = assembleCompositeDetections(meas, measCov, attribs);

    // Assemble info
    info_t info = {costMatrix, globalAssignments, falseAlarmIDs};

    return std::make_tuple(compositeDetections, info);
}
void staticDetectionFuser::fuser_initilization(const std::vector<objectDetection> &detections)
{
    xt::xarray<double> state;
    std::tie(state, std::ignore) = this->pMeasurementFusionFcn(detections, false);
    this->pStateSize = state.size();
}
void staticDetectionFuser::validateTimeStamps(const std::vector<objectDetection> &detections)
{
    double measTime = detections.at(0).Time;
    for (const objectDetection &det : detections)
    {
        if (std::abs(det.Time - measTime) > pTimeTolerance)
        {
            XTENSOR_THROW(std::runtime_error, "fusion:staticDetectionFuser:expectedTimeStampsWithinTolerance");
        }
    }
	this->pDetectionTime = measTime;
}
std::tuple<std::vector<std::vector<objectDetection>>,
           std::vector<std::vector<size_t>>>
staticDetectionFuser::sortDetectionsBySource(const std::vector<objectDetection> &detections)
{
    // pDetections is a cell array of length n, where n is the
    // number of originating sensors and each element is a cell
    // array of objectDetections from that sensor.

    // Validate time stamps are correct
    validateTimeStamps(detections);

    // Sort by originating sensor
    size_t nDets = detections.size();
    xt::xarray<size_t> originatingSensor = xt::zeros<size_t>({nDets});
    for (size_t i = 0; i < nDets; i++)
    {
        originatingSensor.at(i) = detections.at(i).SensorIdx;
    }
    xt::xarray<size_t> sensorIDs = xt::unique(originatingSensor);
    size_t numSensors = sensorIDs.size();

    if (numSensors < 2)
    {
        XTENSOR_THROW(std::runtime_error, "fusion:staticDetectionFuser:expectedAtleast2Sensors");
    }
    if (numSensors > this->pMaxNumSensors)
    {
        XTENSOR_THROW(std::runtime_error, "fusion:staticDetectionFuser:exceedsMaxNumSensors");
    }

    std::vector<std::vector<objectDetection>> pDetections(numSensors);
    std::vector<std::vector<size_t>> globalIDs(numSensors);
    xt::xarray<size_t> theseDetections;
    for (size_t i = 0; i < numSensors; i++)
    {
        theseDetections = xt::flatten_indices(xt::argwhere(xt::equal(originatingSensor, sensorIDs.at(i))));
        for (size_t j = 0; j < theseDetections.size(); j++)
        {
            pDetections.at(i).push_back(detections.at(theseDetections.at(j)));
            globalIDs.at(i).push_back(theseDetections.at(j));
        }
    }
    return std::make_tuple(pDetections, globalIDs);
}

xt::xarray<double> staticDetectionFuser::calcCost(const std::vector<std::vector<objectDetection>> &pDetections)
{
    // Fuse minimum 2 sensors for a valid state estimation.
    size_t minValidTriangulation = 2;

    size_t pDetsLen = pDetections.size();
    xt::xarray<size_t> numDetections = xt::zeros<size_t>({pDetsLen});
    xt::xarray<size_t> sensorIDs = xt::zeros<size_t>({pDetsLen});

    // Collect detections from input and their lengths.
    for (size_t i = 0; i < pDetsLen; i++)
    {
        numDetections.at(i) = pDetections.at(i).size();
        sensorIDs.at(i) = pDetections.at(i).at(0).SensorIdx;
    }

    // sizeValues for cost matrix.
    xt::xarray<size_t> sizeValues = numDetections + 1;

    // Possible number of associations.
    size_t numAssociations = xt::prod(numDetections + 1)(); // 1 for dummy

    // Pre-allocate memory for outputs
    xt::xarray<double> fusedStates = xt::zeros<double>({this->pStateSize, numAssociations});
    xt::xarray<double> costValues = xt::zeros<double>({numAssociations});

    std::vector<size_t> constSizeValues(sizeValues.cbegin(), sizeValues.cend());

    // Get Pd, Pfa, V to pass to the parfor function.
    xt::xarray<double> Pd = xt::xarray<double>({sensorIDs.size()}, this->pDetectionProbability);
    xt::xarray<double> Pfa = xt::xarray<double>({sensorIDs.size()}, this->pFalseAlarmRate);
    xt::xarray<double> V = xt::xarray<double>({sensorIDs.size()}, this->pVolume);

    xt::xarray<double> costMatrix;
    std::tie(std::ignore, costMatrix) = calcNDCostMatrix(fusedStates, costValues, pDetections,
                                                         this->pMeasurementFusionFcn, this->pMeasurementFcn,
                                                         minValidTriangulation, constSizeValues, Pd, Pfa, V);
    return costMatrix;
}

#include "../../assignalgo/include/assignmunkres.h"
#include "../../assignalgo/include/assignmunkres_fast.h"

std::tuple<xt::xarray<size_t>,
           xt::xarray<int>,
           xt::xarray<size_t>>
staticDetectionFuser::calcAssignments(const xt::xarray<double> &costMatrix,
                                      const std::vector<std::vector<size_t>> &globalIDs)
{
    xt::xarray<size_t> assignments;
    if (2 == costMatrix.dimension())
    {
        std::tie(assignments, std::ignore, std::ignore) = assignalgo::assign2DRelaxed(costMatrix, &assignalgo::assignjv);
        assignments = xt::view(assignments, xt::range(1, _), xt::all());
    }
    else
    {
#if 1
		xt::xarray<size_t> assignments_jv, assignments_munkres, assignments_munkres_fast;
		// std::tie(assignments_jv, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignjv);
		std::tie(assignments_munkres, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignmunkres);
		std::tie(assignments_munkres_fast, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignmunkres_fast);
#endif // 0
        std::tie(assignments, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignauction);
    }
    xt::xarray<int> globalAssignments = xt::view(assignments, xt::all(), xt::range(_, globalIDs.size()));
    size_t nRows = globalAssignments.shape(0);
    size_t nCols = globalAssignments.shape(1);
    int getIndex;
    for (size_t j = 0; j < nCols; j++)
    {
        for (size_t i = 0; i < nRows; i++)
        {
            getIndex = globalAssignments.at(i, j) - 1;
            if (getIndex >= 0)
            {
                globalAssignments.at(i, j) = globalIDs.at(j).at(getIndex);
            }
            else
            {
                globalAssignments.at(i, j) = -1;
            }
        }
    }
    xt::xarray<int> numAssignments = xt::sum(globalAssignments >= 0, 1);
    xt::xarray<bool> falseIDs = xt::equal(numAssignments, 1);

    xt::xarray<size_t> rowIdx = xt::arange<size_t>(nRows);
    xt::xarray<int> falseAssociations = xt::sum(xt::view(globalAssignments,
                                                         xt::keep(xt::filter(rowIdx, falseIDs)),
                                                         xt::all()),
                                                1);
    globalAssignments = xt::view(globalAssignments,
                                 xt::keep(xt::filter(rowIdx, !falseIDs)),
                                 xt::all());
    xt::xarray<size_t> falseAlarmIDs = falseAssociations;
    return std::make_tuple(assignments, globalAssignments, falseAlarmIDs);
}
std::tuple<xt::xarray<double>,
           xt::xarray<double>,
           std::vector<std::vector<ObjectAttribute_t>>>
staticDetectionFuser::fuseAssignedDetections(const xt::xarray<size_t> &assignments,
                                             const std::vector<std::vector<objectDetection>> &pDetections)
{
    size_t numMeas = assignments.shape(0);
    double NaN = std::numeric_limits<double>::quiet_NaN();
    xt::xarray<double> meas = xt::xarray<double>({this->pStateSize, numMeas}, NaN);
    xt::xarray<double> measCov = xt::xarray<double>({this->pStateSize, this->pStateSize, numMeas}, NaN);
    std::vector<ObjectAttribute_t> thisFusedAttribute(this->pMaxNumSensors);
    std::vector<std::vector<ObjectAttribute_t>> attribs(numMeas, thisFusedAttribute);
    size_t numSensors = pDetections.size();
    bool fuseAttributes = true;
    size_t retrieveMethod = 1;
    for (size_t i = 0; i < numMeas; i++)
    {
        std::vector<objectDetection> thisBuffer;
        for (size_t j = 0; j < numSensors; j++)
        {
            if (assignments.at(i, j) > 0)
            {
                thisBuffer.push_back(pDetections.at(j).at(assignments.at(i, j) - 1));
            }
        }
        if (thisBuffer.size() >= 2)
        {
            std::tie(xt::col(meas, i), xt::view(measCov, xt::all(), xt::all(), i)) = this->pMeasurementFusionFcn(thisBuffer, true);
        }

        if (fuseAttributes)
        {
            for (size_t k = 0; k < thisBuffer.size(); k++)
            {
                if (1 == retrieveMethod)
                {
                    thisFusedAttribute.at(k) = thisBuffer.at(k).ObjectAttributes.at(0);
                }
                else
                {
                }
            }
        }
        attribs.at(i) = thisFusedAttribute;
    }
    xt::xarray<bool> availableIndices = !xt::isnan(xt::row(meas, 0));
    xt::xarray<size_t> keepIndices = xt::filter(xt::arange<size_t>(numMeas), availableIndices);
    meas = xt::view(meas, xt::all(), xt::keep(keepIndices));
    measCov = xt::view(measCov, xt::all(), xt::all(), xt::keep(keepIndices));
    std::vector<std::vector<ObjectAttribute_t>> fusedAttribs;
    for (size_t i = 0; i < attribs.size(); i++)
    {
        if (availableIndices.at(i))
        {
            fusedAttribs.push_back(attribs.at(i));
        }
    }
    return std::make_tuple(meas, measCov, fusedAttribs);
}
std::vector<objectDetection> staticDetectionFuser::assembleCompositeDetections(const xt::xarray<double> &meas,
                                                                               const xt::xarray<double> &measCov,
                                                                               const std::vector<std::vector<ObjectAttribute_t>> &attribs)
{
    std::vector<objectDetection> compositeDets;
    objectDetection det;
    for (size_t i = 0; i < meas.shape(1); i++)
    {
        det.Time = this->pDetectionTime;
        det.Measurement = xt::col(meas, i);
        det.MeasurementNoise = xt::view(measCov, xt::all(), xt::all(), i);
        det.ObjectAttributes = attribs.at(i);

        compositeDets.push_back(det);
    }
    return compositeDets;
}