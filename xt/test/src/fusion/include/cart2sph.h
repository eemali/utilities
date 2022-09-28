#ifndef CART2SPH_H
#define CART2SPH_H
#include <xtensor/xarray.hpp>
#include <tuple>
std::tuple<xt::xarray<double>,
           xt::xarray<double>,
           xt::xarray<double>>
cart2sph(const xt::xarray<double> &x,
         const xt::xarray<double> &y,
         const xt::xarray<double> &z);
#endif // !CART2SPH_H
