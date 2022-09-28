#include "../include/assignkbest.h"
#include "../include/assignauction.h"
#include "../include/assignjv.h"
#include "../include/assignmunkres.h"
#include "../include/lapCheckCostMatrix.h"
#include "../include/lapCheckUnassignedCost.h"
#include "../include/lapPadForUnassignedRowsAndColumns.h"
#include "../include/kbestGet2DSolution.h"
#include "../include/kbestPartitionSolution.h"
#include "../include/kbestRankSolutions.h"
#include "../include/kbestRemoveDuplicates.h"
#include "../include/kbestRemoveUnassigned.h"
#include <xtensor/xadapt.hpp>
#include <tuple>
#include <deque>
namespace assignalgo
{
    void assignkbest(xt::xarray<double> costMatrix,
                     double costOfNonAssignment,
                     std::vector<xt::xarray<size_t>> &assignments,
                     std::vector<xt::xarray<size_t>> &unassignedRows,
                     std::vector<xt::xarray<size_t>> &unassignedColumns,
                     std::vector<double> &cost,
                     size_t numSolutions,
                     algFcn_t algFcn)
    {
        // validate costMatrix
        lapCheckCostMatrix(costMatrix);

        // validate cost of non-assignment
        lapCheckUnassignedCost(costOfNonAssignment);

        // validate 2D assignment algorithm
        assert(&assignauction == algFcn ||
               &assignjv == algFcn ||
               &assignmunkres == algFcn);

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
        kbestGet2DSolution(partitionCostMatrix, costOfNonAssignment, algFcn, optimalAssignment, &optimalCost);

        // Format of solutionsList
        // 1st element: constrained cost matrix
        // 2nd element: solution to constrained cost matrix
        // 3rd element: cost of assignment
        // 4th element: number of assignments in solutions which were enforced.

        typedef std::tuple<xt::xarray<double>, xt::xarray<size_t>, std::vector<double>, size_t> solution_t;
		solution_t solution(partitionCostMatrix, optimalAssignment, { optimalCost }, 0);
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
        assignments = std::vector<xt::xarray<size_t>>(numSolnFound, xt::zeros<size_t>({0, 2}));
        unassignedRows = std::vector<xt::xarray<size_t>>(numSolnFound, xt::zeros<size_t>({0}));
        unassignedColumns = std::vector<xt::xarray<size_t>>(numSolnFound, xt::zeros<size_t>({0}));
        cost = std::vector<double>(numSolnFound, std::numeric_limits<double>::infinity());

        std::deque<solution_t> foundSolutions(optimalSolutions.cbegin(), optimalSolutions.cbegin() + numSolnFound);
        kbestRankSolutions(foundSolutions);

        for (size_t i = 0; i < numSolnFound; ++i)
        {
            assignments.at(i) = std::get<1>(foundSolutions.at(i));
            kbestRemoveUnassigned(assignments.at(i),
                                  costSize,
                                  &unassignedRows.at(i),
                                  &unassignedColumns.at(i));
            cost.at(i) = std::get<2>(foundSolutions.at(i))[0];
        }
    }
}