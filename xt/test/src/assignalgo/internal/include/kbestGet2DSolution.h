#ifndef K_BEST_GET_2D_SOLUTION_H
#define K_BEST_GET_2D_SOLUTION_H
#include "assigntypedef.h"
namespace assignalgo
{
    /**
     * @brief Calculates solution for 2-D assignment problem with 
     * a cost matrix padded with dummy rows and columns for unassignment. 
     * 
     * @param costMatrix is a padded cost matrix with dummies for unassigned rows and
     * columns. 
     * 
     * @param costOfNonAssignment is a large number greater than the costMatrix. This
     * is to ensure that the assignment functions are called with the required
     * syntax.
     * As cost of non-assignment is included in the matrix, any solution which
     * picks up this cost is declared as invalid. Invalid solutions occur when
     * more constraints are enforced on the costMatrix and hence no feasible
     * solutions can occur.
     * 
     * @param algFcn is the 2-D solution algorithm used
     * 
     * @return (solution,cost,isValidSoln) three-element tuple.
     * The three element of tuple are of the type xt::xarray<size_t>,
     * double and bool respectively
     * 
     * The three-element of tuple are
     * 1. solution - A P-by-2 list of assignments 
     * 2. cost - cost of assignment of solution.
     * 3. isValidSoln - A flag indicating if the solution is valid.
     */
    std::tuple<xt::xarray<size_t>, double, bool>
    kbestGet2DSolution(xt::xarray<double> costMatrix,
                       double costOfNonAssignment,
                       assign2dFcn_t algFcn);
}
#endif // !K_BEST_GET_2D_SOLUTION_H
