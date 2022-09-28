#include "../include/assignkbest.h"
#include "../include/assignauction.h"
#include "../include/assignjv.h"
#include "../include/assignmunkres.h"
#include "../include/assignmunkres_fast.h"
#include "../internal/include/lapCheckCostMatrix.h"
#include "../internal/include/lapCheckUnassignedCost.h"
#include "../internal/include/lapPadForUnassignedRowsAndColumns.h"
#include "../internal/include/kbestGet2DSolution.h"
#include "../internal/include/kbestPartitionSolution.h"
#include "../internal/include/kbestRankSolutions.h"
#include "../internal/include/kbestRemoveDuplicates.h"
#include "../internal/include/kbestRemoveUnassigned.h"
#include <xtensor/xadapt.hpp>
#include <tuple>
#include <deque>
namespace assignalgo
{
    std::tuple<std::vector<xt::xarray<size_t>>,
               std::vector<xt::xarray<size_t>>,
               std::vector<xt::xarray<size_t>>,
               std::vector<double>>
    assignkbest(xt::xarray<double> costMatrix,
                double costOfNonAssignment,
                size_t numSolutions,
                assign2dFcn_t algFcn)
    {
        // validate costMatrix
        lapCheckCostMatrix(costMatrix);

        // validate cost of non-assignment
        lapCheckUnassignedCost(costOfNonAssignment);

        // validate 2D assignment algorithm
        assert(&assignauction == algFcn ||
               &assignjv == algFcn ||
               &assignmunkres == algFcn ||
               &assignmunkres_fast == algFcn);

        // Pad the cost matrix when it's 2-D assignment to cover unassigned rows
        // and columns
        xt::xarray<double> partitionCostMatrix = lapPadForUnassignedRowsAndColumns(costMatrix, costOfNonAssignment);

        // Cost of non-assignment is already included in the partitionCostMatrix.
        // Hence it can be set to a very high value now without any effect. This is
        // to validate the solutions. If a solution picks this cost of assignment,
        // it is invalid.
        costOfNonAssignment = xt::any(xt::isfinite(costMatrix)) * 10e10;

        xt::xarray<size_t> optimalAssignment;
        double optimalCost;
        std::tie(optimalAssignment, optimalCost, std::ignore) =
            kbestGet2DSolution(partitionCostMatrix, costOfNonAssignment, algFcn);

        // Format of solutionsList
        // 1st element: constrained cost matrix
        // 2nd element: solution to constrained cost matrix
        // 3rd element: cost of assignment
        // 4th element: number of assignments in solutions which were enforced.

        typedef std::tuple<xt::xarray<double>, xt::xarray<size_t>, std::vector<double>, size_t> solution_t;
        solution_t solution(partitionCostMatrix, optimalAssignment, {optimalCost}, 0);
        std::deque<solution_t> solutionList = {solution};
        std::deque<solution_t> optimalSolutions(numSolutions, solution);

        std::deque<solution_t> tempSolutionList;
        xt::xarray<size_t> costSize = xt::adapt(costMatrix.shape());
        // Sweep N times to obtain the N-best solutions
        size_t currentSweep = 0;
        while (!solutionList.empty() && currentSweep < numSolutions)
        {
            // Pick up best solution from Queue. It's at the top.
            // Partition the solution.

            tempSolutionList = kbestPartitionSolution(solutionList.at(0), costOfNonAssignment, algFcn);
            for (solution_t s : tempSolutionList)
            {
                solutionList.push_back(s);
            }

            // The partitioned solution may contain the exact solution which is
            // partitioned. This can result in duplicates.
            solutionList = kbestRemoveDuplicates(solutionList, costSize);

            // Rank the solutions in increasing order of cost
            kbestRankSolutions(solutionList);

            // Remove the first solution from the queue and store it to optimal
            // solutions.
            optimalSolutions.at(currentSweep) = solutionList.at(0);
            solutionList.pop_front();

            // Update sweep
            currentSweep = currentSweep + 1;
        }

        size_t numSolnFound = currentSweep;

        // Allocate memory for assignment
        std::vector<xt::xarray<size_t>> assignments(numSolnFound, xt::zeros<size_t>({0, 2}));
        std::vector<xt::xarray<size_t>> unassignedRows(numSolnFound, xt::zeros<size_t>({0}));
        std::vector<xt::xarray<size_t>> unassignedColumns(numSolnFound, xt::zeros<size_t>({0}));
        std::vector<double> cost(numSolnFound, std::numeric_limits<double>::infinity());

        std::deque<solution_t> foundSolutions(optimalSolutions.cbegin(), optimalSolutions.cbegin() + numSolnFound);
        kbestRankSolutions(foundSolutions);

        for (size_t i = 0; i < numSolnFound; ++i)
        {
			assignments.at(i) = std::get<1>(foundSolutions.at(i));
            std::tie(assignments.at(i), unassignedRows.at(i), unassignedColumns.at(i)) =
                kbestRemoveUnassigned(assignments.at(i),
                                      costSize);
            cost.at(i) = std::get<2>(foundSolutions.at(i))[0];
        }
        return std::make_tuple(std::move(assignments),
                               std::move(unassignedRows),
                               std::move(unassignedColumns),
                               std::move(cost));
    }
}