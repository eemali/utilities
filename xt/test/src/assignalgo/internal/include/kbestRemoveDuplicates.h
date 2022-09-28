#ifndef K_BEST_REMOVE_DUPLICATES_H
#define K_BEST_REMOVE_DUPLICATES_H
#include "assigntypedef.h"
#include <deque>
namespace assignalgo
{
    /**
     * @brief Remove duplicate solutions from the solutionList.
     * 
     * @param solutionList - current list of solutions organized as:
     * 1st tulpe element: constrained cost matrix
     * 2nd tulpe element: solution to constrained cost matrix
     * 3rd tulpe element: cost of assignment
     * 4th tulpe element: number of assignments in solutions which were enforced. 
     * 
     * @param costSize - size of the original cost matrix.
     */
    std::deque<solution_t> kbestRemoveDuplicates(const std::deque<solution_t> &solutionList,
                                                 const xt::xarray<size_t> &costSize);
}
#endif // !K_BEST_REMOVE_DUPLICATES_H
