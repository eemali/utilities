#ifndef ASSIGN_MUNKRES_FAST_H
#define ASSIGN_MUNKRES_FAST_H
#include <xtensor/xarray.hpp>
#include <tuple>
namespace assignalgo
{
    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    assignmunkres_fast(xt::xarray<double> costMatrix,
                       double costOfNonAssignment);
}
#endif // !ASSIGN_MUNKRES_FAST_H