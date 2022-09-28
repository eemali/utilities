#ifndef OBJECT_DETECTION_H
#define OBJECT_DETECTION_H
#include <xtensor/xarray.hpp>
enum Frame_t
{
    SPHERICAL,
    RECTANGULAR
};
struct MeasurementParameters_t
{
    Frame_t Frame;
    xt::xarray<double> OriginPosition;
    xt::xarray<double> OriginVelocity;
    xt::xarray<double> Orientation;
    bool IsParentToChild;
    bool HasAzimuth;
    bool HasElevation;
    bool HasRange;
    bool HasVelocity;
    xt::xarray<double> EmitterPosition;
};
struct ObjectAttribute_t
{
    size_t TargetIndex;
    size_t EmitterIndex;
    size_t WaveFormType;
    double SNR;
};
struct objectDetection
{
    double Time;
    xt::xarray<double> Measurement;
    xt::xarray<double> MeasurementNoise;
    size_t SensorIdx;
    size_t ObjectClassID;
    std::vector<MeasurementParameters_t> MeasurementParameters;
    std::vector<ObjectAttribute_t> ObjectAttributes;
};
#endif // !OBJECT_DETECTION_H
