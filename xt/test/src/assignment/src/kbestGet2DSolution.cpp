#include "../include/kbestGet2DSolution.h"
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
namespace assignalgo
{
    static bool isempty(const xt::xarray<size_t> &arr);
    static double calcCost(const xt::xarray<double> &costMatrix,
                           const xt::xarray<size_t> &assignment,
                           const xt::xarray<size_t> &unassignedRows,
                           const xt::xarray<size_t> &unassignedColumns,
                           double costOfNonAssignment);

    void kbestGet2DSolution(xt::xarray<double> costMatrix,
                            double costOfNonAssignment,
                            algFcn_t algFcn,
                            xt::xarray<size_t> &solution,
                            double *cost,
                            bool *isValidSoln)
    {
        xt::xarray<size_t> unassignedRows, unassignedColumns;
        algFcn(costMatrix, costOfNonAssignment, solution, unassignedRows, unassignedColumns);

        if (nullptr != isValidSoln)
        {
            *isValidSoln = true;
            // Because cost of non-assignment is included in the cost matrix, every
            // assignment which could not assign all rows and columns is not a valid
            // solution.

            if (!isempty(unassignedRows) || !isempty(unassignedColumns))
            {
                *isValidSoln = false;
            }
        }

        if (nullptr != cost)
        {
            *cost = calcCost(costMatrix, solution, unassignedRows, unassignedColumns, costOfNonAssignment);
        }

		// CAVEAT: sortrows
		solution = xt::view(solution, xt::keep(xt::argsort(xt::col(solution, 0))));
    }

    static bool isempty(const xt::xarray<size_t> &arr)
    {
        return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
    }

    static double calcCost(const xt::xarray<double> &costMatrix,
                           const xt::xarray<size_t> &assignment,
                           const xt::xarray<size_t> &unassignedRows,
                           const xt::xarray<size_t> &unassignedColumns,
                           double costOfNonAssignment)
    {
        double cost = 0;
        for (size_t i = 0; i < assignment.shape(0); ++i)
        {
            cost += costMatrix.at(assignment.at(i, 0), assignment.at(i, 1));
        }
        cost += (unassignedRows.size() + unassignedColumns.size()) * costOfNonAssignment;
        return cost;
    }
}