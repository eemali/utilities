#ifndef ASSIGN_AUCTION_H
#define ASSIGN_AUCTION_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	void assignauction(xt::xarray<double> costMatrix,
					   double costOfNonAssignment,
					   xt::xarray<size_t> &assignments,
					   xt::xarray<size_t> &unassignedRows,
					   xt::xarray<size_t> &unassignedColumns);
}
#endif // !ASSIGN_AUCTION_H
