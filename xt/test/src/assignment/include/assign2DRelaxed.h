#ifndef ASSIGN_2D_RELAXED_H
#define ASSIGN_2D_RELAXED_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    typedef void (*algFcn_t)(xt::xarray<double>,
                             double,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &);
    xt::xarray<size_t> assign2DRelaxed(xt::xarray<double> costMatrix,
                                       algFcn_t algFcn,
                                       double *cost = nullptr,
                                       xt::xarray<double> *price = nullptr);
}
#endif // !ASSIGN_2D_RELAXED_H