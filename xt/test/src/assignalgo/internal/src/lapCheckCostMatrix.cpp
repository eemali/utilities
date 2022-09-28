#include "../include/lapCheckCostMatrix.h"
namespace assignalgo
{
    void lapCheckCostMatrix(const xt::xarray<double> &costMatrix)
    {
        if (2 != costMatrix.dimension())
        {
            XTENSOR_THROW(std::runtime_error, "costMatrix must be 2D matrix");
        }
        if (xt::any(xt::isnan(costMatrix)))
        {
            XTENSOR_THROW(std::runtime_error, "all element of thecostMatrix must be nonnan");
        }
    }
}
