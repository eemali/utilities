#include "../include/lapCheckCostMatrix.h"
namespace assignalgo
{
    void lapCheckCostMatrix(const xt::xarray<double> &costMatrix)
    {
        assert(2 == costMatrix.shape().size() && !xt::any(xt::isnan(costMatrix)));
    }
}
