#include "../include/kbestGetSDSolution.h"
#include "../../include/assignsd.h"
namespace assignalgo
{
    std::tuple<xt::xarray<size_t>, double, double, bool>
    kbestGetSDSolution(xt::xarray<double> costMatrix,
                       double desiredGap,
                       size_t maxIterations,
                       assign2dFcn_t algFcn)
    {
        xt::xarray<size_t> soln;
        double cost, gap;
        std::tie(soln, cost, gap) = assignsd(costMatrix, desiredGap, maxIterations, algFcn);
        bool isValid = std::isfinite(cost);
        return std::make_tuple(std::move(soln), cost, gap, isValid);
    }
}