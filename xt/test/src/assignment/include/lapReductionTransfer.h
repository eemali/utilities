#ifndef LAP_REDUCTION_TRANSFER
#define LAP_REDUCTION_TRANSFER
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	void lapReductionTransfer(xt::xarray<double> &costMatrix,
							  xt::xarray<double> &rowSoln,
							  xt::xarray<double> &colRedux,
							  xt::xarray<size_t> &rowAssignedCnt);
}
#endif // !LAP_REDUCTION_TRANSFER
