#include "../include/kbestGet2DSolution.h"
#include "../include/xsortrows.h"
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
namespace assignalgo
{
    // Supporting Functions
    // --------------------
    static double calcCost(const xt::xarray<double> &costMatrix,
                           const xt::xarray<size_t> &assignment,
                           const xt::xarray<size_t> &unassignedRows,
                           const xt::xarray<size_t> &unassignedColumns,
                           double costOfNonAssignment);

    std::tuple<xt::xarray<size_t>, double, bool>
    kbestGet2DSolution(xt::xarray<double> costMatrix,
                       double costOfNonAssignment,
                       assign2dFcn_t algFcn)
    {
        xt::xarray<size_t> solution, unassignedRows, unassignedColumns;
        std::tie(solution, unassignedRows, unassignedColumns) = algFcn(costMatrix, costOfNonAssignment);

        bool isValidSoln = true;

        // Because cost of non-assignment is included in the cost matrix, every
        // assignment which could not assign all rows and columns is not a valid
        // solution.

        if (!isempty(unassignedRows) || !isempty(unassignedColumns))
        {
            isValidSoln = false;
        }

        double cost = calcCost(costMatrix, solution, unassignedRows, unassignedColumns, costOfNonAssignment);

        // CAVEAT: sortrows
		solution = xt::sortrows(solution);
        // solution = xt::view(solution, xt::keep(xt::argsort(xt::col(solution, 0))));

        return std::make_tuple(std::move(solution), cost, isValidSoln);
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