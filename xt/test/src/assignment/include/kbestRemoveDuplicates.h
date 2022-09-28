#ifndef K_BEST_REMOVE_DUPLICATES_H
#define K_BEST_REMOVE_DUPLICATES_H
#include <xtensor/xarray.hpp>
#include <deque>
namespace assignalgo
{
	typedef std::tuple<xt::xarray<double>, xt::xarray<size_t>, std::vector<double>, size_t> solution_t;
    std::deque<solution_t> kbestRemoveDuplicates(const std::deque<solution_t> &solutionList,
                                                 const xt::xarray<size_t> &costSize);
}
#endif // !K_BEST_REMOVE_DUPLICATES_H
