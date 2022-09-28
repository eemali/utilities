#ifndef ASSIGN_MUNKRES_H
#define ASSIGN_MUNKRES_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    void assignmunkres(xt::xarray<double> costMatrix,
                       double costOfNonAssignment,
                       xt::xarray<size_t> &assignments,
                       xt::xarray<size_t> &unassignedRows,
                       xt::xarray<size_t> &unassignedColumns);
}
#endif // !ASSIGN_MUNKRES_H