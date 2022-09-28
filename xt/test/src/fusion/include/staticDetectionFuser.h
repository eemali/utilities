#ifndef STATIC_DETECTION_FUSER_H
#define STATIC_DETECTION_FUSER_H
#include "objectDetection.h"
#include "calcNDCostMatrix.h"
#include <tuple>
struct info_t
{
    xt::xarray<double> CostMatrix;
    xt::xarray<size_t> Assignments;
    xt::xarray<size_t> FalseAlarms;
};
class staticDetectionFuser
{
public:
    staticDetectionFuser(stateEstimationFcn_t stateEstimationFcn,
                         measurementFcn_t measurementFcn,
                         size_t MaxNumSensors = 3,
                         size_t FusedSensorIndex = 1,
                         double DetectionProbability = 0.9,
                         double FalseAlarmRate = 1e-6,
                         double Volume = 1e-2,
                         double TimeTolerance = 1e-6);
    std::tuple<std::vector<objectDetection>, info_t> fuse(const std::vector<objectDetection> &detections);

private:
    void fuser_initilization(const std::vector<objectDetection> &detections);
    void validateTimeStamps(const std::vector<objectDetection> &detections);

    std::tuple<std::vector<std::vector<objectDetection>>,
               std::vector<std::vector<size_t>>>
    sortDetectionsBySource(const std::vector<objectDetection> &detections);
    // Calculate cost matrix
    xt::xarray<double> calcCost(const std::vector<std::vector<objectDetection>> &pDetections);
    // Calculate assignments
    std::tuple<xt::xarray<size_t>,
               xt::xarray<int>,
               xt::xarray<size_t>>
    calcAssignments(const xt::xarray<double> &costMatrix,
                    const std::vector<std::vector<size_t>> &globalIDs);
    // Fuse measurement and measurement noise
    std::tuple<xt::xarray<double>,
               xt::xarray<double>,
               std::vector<std::vector<ObjectAttribute_t>>>
    fuseAssignedDetections(const xt::xarray<size_t> &assignments,
                           const std::vector<std::vector<objectDetection>> &pDetections);
    // Assemble composite detections
    std::vector<objectDetection> assembleCompositeDetections(const xt::xarray<double> &meas,
                                                             const xt::xarray<double> &measCov,
                                                             const std::vector<std::vector<ObjectAttribute_t>> &attribs);

private:
    stateEstimationFcn_t pMeasurementFusionFcn;
    measurementFcn_t pMeasurementFcn;
    size_t pMaxNumSensors;
    size_t pFusedSensorIndex;
    double pDetectionProbability;
    double pFalseAlarmRate;
    double pVolume;
    double pTimeTolerance;
    bool pIsInitialized;
    size_t pStateSize;
	double pDetectionTime;
    xt::xarray<size_t> (*p2DAssignmentFcn)(xt::xarray<double> costMatrix);
    xt::xarray<size_t> (*pSDAssignmentFcn)(xt::xarray<double> costMatrix);
};
#endif // !STATIC_DETECTION_FUSER_H
