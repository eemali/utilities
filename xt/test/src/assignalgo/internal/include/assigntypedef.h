#ifndef ASSIGN_TYPE_DEF_H
#define ASSIGN_TYPE_DEF_H
#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <tuple>
namespace assignalgo
{
	typedef std::tuple<xt::xarray<size_t>, // ASSIGNMENTS
					   xt::xarray<size_t>, // UNASSIGNEDROWS
					   xt::xarray<size_t>> // UNASSIGNEDCOLUMNS
		(*assign2dFcn_t)(xt::xarray<double>,
						 double);

	typedef std::tuple<xt::xarray<double>,
					   xt::xarray<size_t>,
					   std::vector<double>, size_t>
		solution_t;

	// Supporting Functions
	// --------------------
	/**
 	 * Is input array empty?
 	 */
	template <typename T>
	bool isempty(const xt::xarray<T> &arr)
	{
		return ((0 == arr.dimension()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
	}
}
#endif // !ASSIGN_TYPE_DEF_H
