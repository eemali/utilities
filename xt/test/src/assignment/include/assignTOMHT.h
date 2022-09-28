#ifndef ASSIGN_TOMHT_H
#define ASSIGN_TOMHT_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    void assignTOMHT(xt::xarray<double> &costMatrix,
                     xt::xarray<double> &costThreshold,
                     xt::xarray<size_t> &assignments,
                     xt::xarray<size_t> &unassignedTracks,
                     xt::xarray<size_t> &unassignedDetections);
}
#endif // !ASSIGN_TOMHT_H
