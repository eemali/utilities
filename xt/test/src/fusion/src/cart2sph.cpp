#include "../include/cart2sph.h"
std::tuple<xt::xarray<double>,
           xt::xarray<double>,
           xt::xarray<double>>
cart2sph(const xt::xarray<double> &x,
         const xt::xarray<double> &y,
         const xt::xarray<double> &z)
{
    xt::xarray<double> hypotxy = xt::sqrt(xt::pow(x, 2) + xt::pow(y, 2));
    xt::xarray<double> r = xt::sqrt(xt::pow(hypotxy, 2) + xt::pow(z, 2));
    xt::xarray<double> elev = xt::atan2(z, hypotxy);
    xt::xarray<double> az = xt::atan2(y, x);
    return std::make_tuple(az, elev, r);
}