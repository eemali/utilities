#include "../include/kbestGetSDSolution.h"
#include "../include/assignsd.h"
namespace assignalgo
{
    void kbestGetSDSolution(xt::xarray<double> costMatrix,
                            double desiredGap,
                            size_t maxIterations,
                            algFcn_t algFcn,
                            xt::xarray<size_t> &soln,
                            double *cost,
                            double *gap,
                            bool *isValid)
    {
        assignsd(costMatrix, soln, cost, gap, desiredGap, maxIterations, algFcn);
        if (nullptr != isValid)
        {
			assert(nullptr != cost);
            *isValid = std::isfinite(*cost);
        }
    }
}