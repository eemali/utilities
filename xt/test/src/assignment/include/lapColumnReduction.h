#ifndef LAP_COLUMN_REDUCTION_H
#define LAP_COLUMN_REDUCTION_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	void lapColumnReduction(xt::xarray<double> &costMatrix,
							xt::xarray<double> &rowSoln,
							xt::xarray<double> &colSoln,
							xt::xarray<double> &colRedux,
							xt::xarray<size_t> &rowAssignedCnt);
}
#endif // !LAP_COLUMN_REDUCTION_H
