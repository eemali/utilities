#include "../include/sph2cart.h"
std::tuple<xt::xarray<double>,
           xt::xarray<double>,
           xt::xarray<double>>
sph2cart(const xt::xarray<double> &az,
         const xt::xarray<double> &elev,
         const xt::xarray<double> &r)
{
    xt::xarray<double> z = r * xt::sin(elev);
    xt::xarray<double> rcoselev = r * xt::cos(elev);
    xt::xarray<double> x = rcoselev * xt::cos(az);
    xt::xarray<double> y = rcoselev * xt::sin(az);
    return std::make_tuple(x, y, z);
}