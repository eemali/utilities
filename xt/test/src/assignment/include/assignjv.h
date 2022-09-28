#ifndef ASSIGN_JONKER_VOLGENANT_H
#define ASSIGN_JONKER_VOLGENANT_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	void assignjv(xt::xarray<double> costMatrix,
				  double costOfNonAssignment,
				  xt::xarray<size_t> &assignments,
				  xt::xarray<size_t> &unassignedRows,
				  xt::xarray<size_t> &unassignedColumns);
}
#endif // !ASSIGN_JONKER_VOLGENANT_H
