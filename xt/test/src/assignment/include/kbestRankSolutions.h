#ifndef K_BEST_RANK_SOLUTIONS
#define K_BEST_RANK_SOLUTIONS
#include <xtensor/xarray.hpp>
#include <deque>
namespace assignalgo
{
	typedef std::tuple<xt::xarray<double>, xt::xarray<size_t>, std::vector<double>, size_t> solution_t;
	void kbestRankSolutions(std::deque<solution_t> &solutionList);
}
#endif // !K_BEST_RANK_SOLUTIONS
