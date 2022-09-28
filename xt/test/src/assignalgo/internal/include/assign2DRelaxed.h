#ifndef ASSIGN_2D_RELAXED_H
#define ASSIGN_2D_RELAXED_H
#include "assigntypedef.h"
namespace assignalgo
{
  /**
     * @brief Solve a 2-D assignment problem with 0 index assigned to
     * the dummy.
     * 
     * Solves a
     * generalized assignment problem where the index 0 is reserved for dummies.
     * Algorithm can be specified as a function handle from the following list:
     * &assignauction, &assignjv, &assignmunkres.
     * 
     * It also allows to retrieve the price or dual variables from the auction
     * algorithm. If algorithm specified is other than &assignauction, the price
     * is returned as zeros.
     * 
     * The first assignment is always a dummy solution i.e. [0 0]. 
     * In the optimal solution, [0 0] is included if costMatrix(0,0) < 0
     */
  std::tuple<xt::xarray<size_t>,
             double,
             xt::xarray<double>>
  assign2DRelaxed(xt::xarray<double> costMatrix,
                  assign2dFcn_t algFcn);
}
#endif // !ASSIGN_2D_RELAXED_H