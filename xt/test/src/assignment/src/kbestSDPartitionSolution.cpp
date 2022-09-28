#include "../include/kbestSDPartitionSolution.h"
#include "../include/kbestEnforceConstraints.h"
#include "../include/kbestGetSDSolution.h"
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    std::deque<solution_t> kbestSDPartitionSolution(const solution_t &solution,
                                                    double desiredGap,
                                                    size_t maxIterations,
                                                    algFcn_t algFcn)
    {
        // Get current assignment, costMatrix and number of enforced assignments
        xt::xarray<double> costMatrix = std::get<0>(solution);
        xt::xarray<size_t> assignment = std::get<1>(solution);
        size_t enforcedConstraintIndex = 0; // std::get<3>(solution);

        // The partition of assignment only takes places on "chosen" assignments and
        // not forced assignments
        xt::xarray<size_t> toPartitionAssignment = xt::view(assignment, xt::range(enforcedConstraintIndex, _));
        int numPartitions = toPartitionAssignment.shape(0);

        // Create memory for partitioned solution.
        std::deque<solution_t> partitionedSolution;

        xt::xarray<size_t> tuplesToRemove, tuplesToEnforce, newSolution;
        xt::xarray<double> constrainedCost;
        double newCost, newGap;
        bool isValidSoln;
        for (int k = 0; k < numPartitions; ++k)
        {
            // Remove kth tuple from toPartitionAssignment
            tuplesToRemove = xt::view(toPartitionAssignment, xt::range(k, k + 1));

            // Enforce first k-1 tuples from toPartitionAssignment
            tuplesToEnforce = xt::view(toPartitionAssignment, xt::range(_, k));

            // Enforce constraints
            constrainedCost = kbestEnforceConstraints(costMatrix, tuplesToRemove, tuplesToEnforce);

            // Solve the new problem
            kbestGetSDSolution(constrainedCost, desiredGap, maxIterations, algFcn, newSolution, &newCost, &newGap, &isValidSoln);

            // Store the solution in list
            if (isValidSoln)
            {
				partitionedSolution.emplace_back(solution_t(constrainedCost, newSolution, { newCost, newGap }, enforcedConstraintIndex + k));
            }
        }
        return partitionedSolution;
    }
}