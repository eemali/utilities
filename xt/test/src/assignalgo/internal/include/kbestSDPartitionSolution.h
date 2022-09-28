#ifndef K_BEST_SD_PARTITION_SOLUTION
#define K_BEST_SD_PARTITION_SOLUTION
#include "assigntypedef.h"
#include <deque>
namespace assignalgo
{
    /**
     * @brief partition an SD solution into multiple lists based on
     * Murty's algorithm.
     * 
     * * @param solution - a four-element tulpe organized as follows:
     * 1st tulpe element: constrained cost matrix
     * 2nd tulpe element: solution to constrained cost matrix
     * 3rd tulpe element: cost and gap of assignment
     * 4th tulpe element: number of assignments in solutions which were enforced.
     * 
     * desiredGap, maxIterations, algorithm are the same as the inputs to assignkbestsd
     * 
     * @return An array of solutions partitioned from the input solution organized in
     * the same format.
     */
    std::deque<solution_t> kbestSDPartitionSolution(const solution_t &solution,
                                                    double desiredGap,
                                                    size_t maxIterations,
                                                    assign2dFcn_t algFcn);
}
#endif // !K_BEST_SD_PARTITION_SOLUTION