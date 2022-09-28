#include "../include/global2localcoord.h"
#include "../include/sph2cart.h"
#include "../include/cart2sph.h"
#include <xtensor/xview.hpp>
#include <xtensor-blas/xlinalg.hpp>
using namespace xt::placeholders;
xt::xarray<double> global2localcoord(xt::xarray<double> gCoord,
                                     const std::string option,
                                     const xt::xarray<double> &localOrigin,
                                     const xt::xarray<double> &localAxes)
{
    xt::xarray<double> gRect;
    if ('r' == option.at(0))
    {
        gRect = gCoord;
    }
    else
    {
        gRect = gCoord; // Allocate a matrix of the correct size and type
        xt::view(gCoord, xt::range(_, 2)) *= xt::numeric_constants<double>::PI / 180.;
        std::tie(xt::row(gRect, 0), xt::row(gRect, 1), xt::row(gRect, 2)) = sph2cart(
            xt::row(gCoord, 0), xt::row(gCoord, 1), xt::row(gCoord, 2));
    }
    // subtract displacement incurred by the platform position; rotate so that
    // the system is local the global [ux; uy; uz] should map to [1; 0; 0] in local
    // note localAxes is [ux vx wx;uy vy wy;uz vz wz];

    xt::xarray<double> lclRect = xt::linalg::dot(xt::transpose(localAxes), gRect - localOrigin);
    lclRect.reshape({-1, 1}); // TODO
    xt::xarray<double> lclCoord;
    if ('r' == option.at(1))
    {
        lclCoord = lclRect;
    }
    else
    {
        lclCoord = lclRect; // Allocate a matrix of the correct size and type
        std::tie(xt::row(lclCoord, 0), xt::row(lclCoord, 1), xt::row(lclCoord, 2)) = cart2sph(
            xt::row(lclRect, 0), xt::row(lclRect, 1), xt::row(lclRect, 2));
        xt::view(lclCoord, xt::range(_, 2)) *= 180.0 / xt::numeric_constants<double>::PI;
    }
    return xt::squeeze(lclCoord); // TODO
}