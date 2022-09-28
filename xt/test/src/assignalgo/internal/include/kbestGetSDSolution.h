#ifndef K_BEST_GET_SD_SOLUTION_H
#define K_BEST_GET_SD_SOLUTION_H
#include "assigntypedef.h"
namespace assignalgo
{
    /**
     * @brief Calculates solution for S-D assignment problem 
     * 
     * @param costMatrix is the S-D cost matrix
     * @param desiredGap is the gap that controls the assignsd convergence
     * @param maxItertions is the maximum number of iterations that assignsd runs
     * @param algFcn is the 2-D solution algorithm used by assignsd
     * 
     * @return (soln, cost, gap, isValid) four-element tuple.
     * The four element of tuple are of the type xt::xarray<size_t>,
     * double, double and bool respectively.
     * 
     * The four-element of tuple are
     * 1. soln - A P-by-S list of assignments (S - dimension of costMatrix).
     * 2. cost - cost of assignment of solution.
     * 3. gap - duality gap of the solution.
     * 4. isValid - A flag indicating if the solution is valid.
     */
    std::tuple<xt::xarray<size_t>, double, double, bool>
    kbestGetSDSolution(xt::xarray<double> costMatrix,
                       double desiredGap,
                       size_t maxIterations,
                       assign2dFcn_t algFcn);
}
#endif // !K_BEST_GET_SD_SOLUTION_H
