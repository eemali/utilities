#ifndef ASSIGN_K_BEST_H
#define ASSIGN_K_BEST_H
#include <xtensor/xarray.hpp>
#include "../include/assignauction.h"
#include <vector>
namespace assignalgo
{
    typedef void (*algFcn_t)(xt::xarray<double>,
                             double,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &);

    void assignkbest(xt::xarray<double> costMatrix,
                     double costOfNonAssignment,
                     std::vector<xt::xarray<size_t>> &assignments,
                     std::vector<xt::xarray<size_t>> &unassignedRows,
                     std::vector<xt::xarray<size_t>> &unassignedColumns,
                     std::vector<double> &cost,
                     size_t numSolutions = 1,
                     algFcn_t algFcn = &assignauction);
}
#endif // !ASSIGN_K_BEST_H
