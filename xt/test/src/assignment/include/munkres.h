#ifndef MUNKRES_H
#define MUNKRES_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    void munkres(xt::xarray<double> costMatrix,
                 double costOfNonAssignment,
                 xt::xarray<size_t> &assignments,
                 xt::xarray<size_t> &unassignedRows,
                 xt::xarray<size_t> &unassignedColumns);
}
#endif // !MUNKRES_H