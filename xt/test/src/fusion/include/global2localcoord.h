#ifndef GLOBAL_TO_LOCAL_COORD_H
#define GLOBAL_TO_LOCAL_COORD_H
#include <xtensor/xarray.hpp>
xt::xarray<double> global2localcoord(xt::xarray<double> gCoord,
                                     const std::string option,
                                     const xt::xarray<double> &localOrigin,
                                     const xt::xarray<double> &localAxes);
#endif // !GLOBAL_TO_LOCAL_COORD_H
