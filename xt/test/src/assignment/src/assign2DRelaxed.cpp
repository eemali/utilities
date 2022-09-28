#include "../include/assign2DRelaxed.h"
#include "../include/assignauction.h"
#include "../include/assignAuctionWithPrice.h"
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xsort.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    static xt::xarray<double> padCostMatrix(const xt::xarray<double> &costMatrix);

    xt::xarray<size_t> assign2DRelaxed(xt::xarray<double> costMatrix,
                                       algFcn_t algFcn,
                                       double *cost,
                                       xt::xarray<double> *price)
    {
        // No validation is performed in this function as it is done in the caller
        // functions

        // The dummy-dummy assignment does not take part in the solution.
        double temp = costMatrix.at(0, 0);
        // Set it's cost to 0 for now.
        costMatrix.at(0, 0) = 0;
        // Assess the size of cost Matrix.
        size_t nRow = costMatrix.shape(0);
        size_t nCol = costMatrix.shape(1);

        // Pad the cost matrix with unassigned rows and columns in a different
        // manner than lapPadForUnassignedRowsAndColumns.
        // In this case the padding is done using the (1,2:end) and (2:end,1) as the
        // cost of unassignment for rows and columns respectively.
        xt::xarray<double> relaxedMatrix = padCostMatrix(costMatrix);

        // If assignauction is used from the top function, divert it to an internal
        // auction algorithm which can return the price variables from the auction
        // algorithm.
        xt::xarray<size_t> assignments, unassignedRows, unassignedCols;
        xt::xarray<double> priceWithDummy;
        if (&assignauction == algFcn)
        {
            assignAuctionWithPrice(relaxedMatrix, assignments, unassignedRows, unassignedCols, priceWithDummy);
        }
        else
        {
            // For all other assignment algorithms, use their respective functions with a
            // big number for cost of non assignment. This is done because cost of non
            // assignment is already padded in the relaxedMatrix.
            algFcn(relaxedMatrix, 1e10, assignments, unassignedRows, unassignedCols);
            // priceWithDummy is assiged to 0 as price has no meaning for non-auction
            // algorithms.
            priceWithDummy = xt::zeros<double>({nCol});
        }
        // Assign unassigned rows and columns to 0.
        assignments = xt::vstack(xt::xtuple(assignments,
                                            xt::stack(xt::xtuple(unassignedRows, xt::zeros_like(unassignedRows)), 1),
                                            xt::stack(xt::xtuple(xt::zeros_like(unassignedCols), unassignedCols), 1)));

        // CAVEAT: sortrows
		assignments = xt::view(assignments, xt::keep(xt::argsort(xt::col(assignments, 0))));

        // Restore original indices from assignment. The assignment takes place from
        // (2:end,2:end) as (1,1) does not take part in the assignment problem.
        assignments = assignments + 1;

        // Modify the priceWithDummy to update the index.
        if (nullptr != price)
        {
            *price = xt::concatenate(xt::xtuple(xt::zeros<double>({1}),
                                                xt::view(priceWithDummy, xt::range(_, nCol - 1))),
                                     0);
        }
        // Remove the padded rows and columns from the solution and restore the
        // actual solution.
        xt::xarray<size_t> colSoln = xt::col(assignments, 0);
        xt::xarray<bool> validRows = colSoln < nRow;
        xt::xarray<size_t> rowSoln = xt::filter(xt::col(assignments, 1), validRows);
        xt::xarray<size_t> rowColSoln = xt::filter(xt::col(assignments, 0), validRows);
        xt::filtration(rowSoln, rowSoln >= nCol) = 0;
        xt::xarray<size_t> dummyRowSoln = xt::filter(xt::col(assignments, 1), !validRows);
        xt::xarray<bool> validCols = dummyRowSoln < nCol;
        dummyRowSoln = xt::filter(dummyRowSoln, validCols);
        xt::xarray<size_t> assign = xt::vstack(xt::xtuple(xt::stack(xt::xtuple(rowColSoln, rowSoln), 1),
                                                          xt::stack(xt::xtuple(xt::zeros_like(dummyRowSoln), dummyRowSoln), 1)));
        //sortrows as assignmunkres does not return sorted rows.
		// CAVEAT: sortrows
		assign = xt::view(assign, xt::keep(xt::argsort(xt::col(assign, 0))));

        // Keep only 1 dummy in the solution.
        xt::xarray<bool> dummyAssignment = xt::equal(xt::col(assign, 0), 0) & xt::equal(xt::col(assign, 1), 0);
        size_t numDummies = xt::sum(dummyAssignment)();
        if (numDummies >= 1)
        {
            assign = xt::view(assign, xt::range(numDummies, _), xt::all());
        }
        // Pad the assignment with [1 1] as this is required for future enforcements
        // of constraints. For example, the third dimension assignment should have a
        // dummy variable to be associated with it n3 > n1 or n3 > n2.
        if (temp <= 0 || true)
        {
            assign = xt::vstack(xt::xtuple(xt::zeros<size_t>({1, 2}), assign));
        }
        // Compute the cost of assignment.
        if (nullptr != cost)
        {
            costMatrix.at(0, 0) = temp;
            for (size_t i = 0; i < assign.shape(0); ++i)
            {
                *cost += costMatrix.at(assign.at(i, 0), assign.at(i, 1));
            }
        }
        return assign;
    }

    static xt::xarray<double> padCostMatrix(const xt::xarray<double> &costMatrix)
    {
        double inf = std::numeric_limits<double>::infinity();
        // This function calculates the padded cost for allowing multiple
        // dimensions on index 1 on each dimension.
        xt::xarray<double> innerCost = xt::view(costMatrix, xt::range(1, _), xt::range(1, _));
        xt::xarray<double> dummyFirst = xt::view(costMatrix, 0, xt::range(1, _));
        xt::xarray<double> dummySecond = xt::view(costMatrix, xt::range(1, _), 0);
        size_t nFirst = dummyFirst.size();
        size_t nSecond = dummySecond.size();
        xt::xarray<double> dummyCostFirst({nFirst, nFirst}, inf);
        xt::xarray<double> dummyCostSecond({nSecond, nSecond}, inf);
        for (size_t idx : xt::arange<size_t>(nFirst))
        {
            dummyCostFirst.at(idx, idx) = dummyFirst.at(idx);
        }
        for (size_t idx : xt::arange<size_t>(nSecond))
        {
            dummyCostSecond.at(idx, idx) = dummySecond.at(idx);
        }
        xt::xarray<double> dummyDummyAssignment({nFirst, nSecond}, 0);
        xt::xarray<double> paddedCost = xt::vstack(xt::xtuple(xt::hstack(xt::xtuple(innerCost, dummyCostSecond)),
                                                              xt::hstack(xt::xtuple(dummyCostFirst, dummyDummyAssignment))));
        return paddedCost;
    }
}