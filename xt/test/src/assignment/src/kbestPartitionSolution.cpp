#include "../include/kbestPartitionSolution.h"
#include "../include/kbestEnforceConstraints.h"
#include "../include/kbestGet2DSolution.h"
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    std::deque<solution_t> kbestPartitionSolution(const solution_t &solution,
                                                  double costOfNonAssignment,
                                                  algFcn_t algFcn)
    {
        // Get current assignment, costMatrix and number of enforced assignments
        xt::xarray<double> costMatrix = std::get<0>(solution);
        xt::xarray<size_t> assignment = std::get<1>(solution);
        size_t enforcedConstraintIndex = std::get<3>(solution);

        // The partition of assignment only takes places on "chosen" assignments and
        // not forced assignments
        xt::xarray<size_t> toPartitionAssignment = xt::view(assignment, xt::range(enforcedConstraintIndex, _));
        int numPartitions = toPartitionAssignment.shape(0) - 1;

        // Create memory for partitioned solution.
        std::deque<solution_t> partitionedSolution;

        xt::xarray<size_t> tuplesToRemove, tuplesToEnforce, newSolution;
        xt::xarray<double> constrainedCost;
        double newCost;
        bool isValidSoln;
        for (int k = 0; k < numPartitions; ++k)
        {
            // Remove kth tuple from toPartitionAssignment
            tuplesToRemove = xt::view(toPartitionAssignment, xt::range(k, k+1));

            // Enforce first k-1 tuples from toPartitionAssignment
            tuplesToEnforce = xt::view(toPartitionAssignment, xt::range(_, k));

            // Enforce constraints
            constrainedCost = kbestEnforceConstraints(costMatrix, tuplesToRemove, tuplesToEnforce);

            // Solve the new problem
            kbestGet2DSolution(constrainedCost, costOfNonAssignment, algFcn, newSolution, &newCost, &isValidSoln);

            // Store the solution in list
            if (isValidSoln)
            {
				partitionedSolution.emplace_back(solution_t(constrainedCost, newSolution, { newCost }, enforcedConstraintIndex + k));
            }
        }
        return partitionedSolution;
    }
}