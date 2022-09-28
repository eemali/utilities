#ifndef SPH2CART_H
#define SPH2CART_H
#include <xtensor/xarray.hpp>
#include <tuple>
std::tuple<xt::xarray<double>,
           xt::xarray<double>,
           xt::xarray<double>>
sph2cart(const xt::xarray<double> &az,
         const xt::xarray<double> &elev,
         const xt::xarray<double> &r);
#endif // !SPH2CART_H
