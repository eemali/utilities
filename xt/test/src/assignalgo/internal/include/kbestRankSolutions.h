#ifndef K_BEST_RANK_SOLUTIONS
#define K_BEST_RANK_SOLUTIONS
#include "assigntypedef.h"
#include <deque>
namespace assignalgo
{
	/**
	 * @brief ranks the solutionList in an increasing order of
	 * cost for k-best the algorithm to pick the top solution for partition.
	 */
	void kbestRankSolutions(std::deque<solution_t> &solutionList);
}
#endif // !K_BEST_RANK_SOLUTIONS
