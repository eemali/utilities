#ifndef GLOBAL_TO_LOCAL_COORD_JAC_H
#define GLOBAL_TO_LOCAL_COORD_JAC_H
#include <xtensor/xarray.hpp>
xt::xarray<double> global2localcoordjac(const xt::xarray<double> &tgtpos,
                                        const xt::xarray<double> &sensorpos,
                                        const xt::xarray<double> &laxes);
#endif // !GLOBAL_TO_LOCAL_COORD_JAC_H
