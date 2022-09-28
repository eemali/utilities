#ifndef K_BEST_PARTITION_SOLUTION
#define K_BEST_PARTITION_SOLUTION
#include "assigntypedef.h"
#include <deque>
namespace assignalgo
{
    /**
     * @brief partition a solution into multiple lists based on
     * Murty's algorithm.
     * 
     * @param solution - a four-element tulpe organized as follows:
     * 1st tulpe element: constrained cost matrix
     * 2nd tulpe element: solution to constrained cost matrix
     * 3rd tulpe element: cost of assignment
     * 4th tulpe element: number of assignments in solutions which were enforced.
     * 
     * @param costOfNonAssignment cost of non-assignment for solving the 2-D
     * assignment problem.
     * 
     * @param algFcn - Algorithm to solve the 2-D assignment problem. 
     * Options are: &assignauction, &assignjv and &assignmunkres.
     * 
     * @return An array of solutions partitioned from the input solution organized in
     * the same format.
     */
    std::deque<solution_t> kbestPartitionSolution(const solution_t &solution,
                                                  double costOfNonAssignment,
                                                  assign2dFcn_t algFcn);
}
#endif // !K_BEST_PARTITION_SOLUTION
