#ifndef HELPER_SPHERICAL_INTERSECTION_H
#define HELPER_SPHERICAL_INTERSECTION_H
#include <xtensor/xarray.hpp>
xt::xarray<double> helperSphericalIntersection(const xt::xarray<double> &r,
                                               xt::xarray<double> receiverLocations,
                                               const xt::xarray<double> &emitterLocation);
#endif // !HELPER_SPHERICAL_INTERSECTION_H
