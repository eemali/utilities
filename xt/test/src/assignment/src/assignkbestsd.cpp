#include "../include/assignkbestsd.h"
#include "../include/assignauction.h"
#include "../include/assignjv.h"
#include "../include/assignmunkres.h"
#include "../include/lapCheckCostMatrix.h"
#include "../include/lapCheckUnassignedCost.h"
#include "../include/lapPadForUnassignedRowsAndColumns.h"
#include "../include/kbestGetSDSolution.h"
#include "../include/kbestSDPartitionSolution.h"
#include "../include/kbestRankSolutions.h"
#include "../include/kbestRemoveDuplicates.h"
#include "../include/kbestRemoveUnassigned.h"
#include <xtensor/xadapt.hpp>
#include <tuple>
#include <deque>
namespace assignalgo
{
    // Supporting Functions
    static bool isempty(const xt::xarray<double> &arr);

    void assignkbestsd(xt::xarray<double> costMatrix,
                       std::vector<xt::xarray<size_t>> &assignments,
                       std::vector<double> &cost,
                       std::vector<double> &gap,
                       size_t numSolutions,
                       double desiredGap,
                       size_t maxIterations,
                       algFcn_t algFcn)
    {
        // Validate Inputs

        // Assess size and type of the problem.
        xt::xarray<size_t> costSize = xt::adapt(costMatrix.shape());
        size_t costDim = costSize.size();

        assert(!xt::any(xt::isnan(costMatrix)) && // nonnan
               !isempty(costMatrix) &&            // nonempty
               costDim >= 3 &&                    // expectedAtleast3D
               xt::all(costSize > 1));            // expectedAtleast2Dims

        // validate 2D assignment algorithm
        assert(&assignauction == algFcn ||
               &assignjv == algFcn ||
               &assignmunkres == algFcn);

        xt::xarray<size_t> assignment;
        double c, g;
        kbestGetSDSolution(costMatrix, desiredGap, maxIterations, algFcn, assignment, &c, &g);

        // Format of solutionsList
        // 1st element: constrained cost matrix
        // 2nd element: solution to constrained cost matrix
        // 3rd element: [cost, gap of assignment]
        // 4th element: number of assignments in solutions which were enforced.

        typedef std::tuple<xt::xarray<double>, xt::xarray<size_t>, std::vector<double>, size_t> solution_t;
        solution_t solution(costMatrix, assignment, {c, g}, 0);
        std::deque<solution_t> solutionList = {solution};
        std::deque<solution_t> optimalSolutions(numSolutions, solution);

        std::deque<solution_t> tempSolutionList, checkSolutionList, removedDupsList;

        // Sweep N times to obtain the N-best solutions
        size_t currentSweep = 0;

        while (!solutionList.empty() && currentSweep < numSolutions)
        {
            // Pick up best solution from Queue. It's at the top.
            // Partition the solution.

            tempSolutionList = kbestSDPartitionSolution(solutionList.at(0), desiredGap, maxIterations, algFcn);
            for (solution_t s : tempSolutionList)
            {
                solutionList.push_back(s);
            }

            // The solutionList may also contain assignments which are already
            // partitioned and stored in optimal list. They should not be
            // partitioned again. The solutionList may itself also contain duplicate
            // partitions. The following code should remove both of these
            // duplicates.
            checkSolutionList = std::deque<solution_t>(optimalSolutions.cbegin(), optimalSolutions.cbegin() + currentSweep);
            for (solution_t s : solutionList)
            {
                checkSolutionList.push_back(s);
            }

            removedDupsList = kbestRemoveDuplicates(checkSolutionList, costSize);
            // The last solutions are what we need as the first currentSweep-1 had
            // no duplicates.
            solutionList = std::deque<solution_t>(removedDupsList.cbegin() + currentSweep, removedDupsList.cend());

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
        cost = std::vector<double>(numSolnFound, std::numeric_limits<double>::infinity());
        gap = std::vector<double>(numSolnFound, 0);

        std::deque<solution_t> foundSolutions(optimalSolutions.cbegin(), optimalSolutions.cbegin() + numSolnFound);
        kbestRankSolutions(foundSolutions);

        for (size_t i = 0; i < numSolnFound; ++i)
        {
            assignments.at(i) = std::get<1>(foundSolutions.at(i));
            cost.at(i) = std::get<2>(foundSolutions.at(i))[0];
            gap.at(i) = std::get<2>(foundSolutions.at(i))[1];
        }
    }
    // Supporting Functions
    //---------------------
    static bool isempty(const xt::xarray<double> &arr)
    {
        return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
    }
}