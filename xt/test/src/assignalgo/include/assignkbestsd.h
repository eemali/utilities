#ifndef ASSIGN_K_BEST_SD_H
#define ASSIGN_K_BEST_SD_H
#include "assignauction.h"
#include "../internal/include/assigntypedef.h"
#include <vector>
namespace assignalgo
{
    /**
     * @brief K-best solutions to the S-D assignment problem
     * 
     * It finds the best S-D solution that minimize the total cost of assignment.
     * The algorithm uses Lagrangian relaxation to convert the S-D assignment
     * problem to a corresponding 2-D assignment problem and then solves the
     * 2-D problem.
     * 
     * @param costMatrix is an n-dimensional cost matrix where costMatrix(i,j,k ...)
     * defines the cost of the n-tuple (i,j,k, ...) in assignment. The index
     * '0' on all dimensions in the costMatrix represents dummy measurement or
     * a false track and is used to complete the assignment problem. The index
     * 0, being a dummy, can be a part of multiple n-tuples, i.e., it is not
     * constrained to be assigned only once. A typical cost value for
     * costMatrix(0,0,0,0,..) is 0.
     * 
     * @param numSolutions finds the numSolutions-best S-D assignment
     * solutions that minimize the total cost of assignment. The algorithm
     * uses Murty's algorithm to find numSolutions-1 solutions, in addition to the best
     * solution. If not specified, the default value for numSolutions is 1.
     * 
     * @param desiredGap specify the desired maximum gap between the dual solution and the
     * feasible solution as a scalar usually ranging between 0 and 1. A value
     * of 0 means the dual and feasible solutions are the same. The gap can be
     * used to control the quality of the solution. If not specified, the
     * default value for the desiredGap is 0.01.

     * @param maxIterations specify the maximum number of iterations the assignsd is allowed.
     * If not specified, the default value for maxIterations is 100.
     *  
     * @param algFcn specify the algorithm to use for solving the 2-D
     * assignment problem. 
     * The choices of algorithm are: &assignauction, &assignjv, &assignmunkres
     * 
     * @return (assignments,cost,gap) three-element tuple
     * The first element of tuple is of the type std::vector<xt::xarray<size_t>>,
     * and last two element of tuple is of the type std::vector<double>.
     * 
     * The three-element of tuple are
     * 1. assignments is a k-element vector. Each cell element is a P-by-N
     * list of assignment returned by the S-D assignment function, assignsd. 
     * Assignments of the type [0 0 Q 0] from a 4-dimensional cost matrix can
     * be seen as Q-1 entity from dimension 3 was left unassigned. The cost
     * value at (0,0,Q,0) defines the cost of not assigning the (Q-1)th entity
     * from dimension 3.
     * 
     * 2. costOfAssignment is a k-element vector. The j-th element is a scalar
     * value summarizing the total cost of the j-th solution to the S-D
     * assignment.
     * 
     * 3. solutionGap is a positive k-element vector. Each element is the duality
     * gap achieved between the feasible and dual solution. A value of
     * solutionGap near zero indicates the quality of solution.
     * 
     * \code{cpp}
     * #include "../../include/assignkbestsd.h"
     *  #include <xtensor/xio.hpp>
     *   int main(int argc, char const *argv[])
     *   {
     *       xt::xarray<double> costMatrix = {{{0, 0, 0},
     *                                       {0, -6.8, -10.9},
     *                                       {0, -4.5, -11.1}},
     *                                       {{0, -10.2, -4.7},
     *                                       {-6.8, -18, -17},
     *                                       {-13.2, 0, -9}},
     *                                       {{0, 0, -5.5},
     *                                       {-5.2, -14.8, -9.9},
     *                                       {-10.6, -14.1, -16.7}}};
     *       costMatrix = xt::transpose(costMatrix, {1, 2, 0});
     *       std::vector<xt::xarray<size_t>> assignments;
     *       std::vector<double> cost, solutionGap;
     *       std::tie(assignments, cost, solutionGap) = assignalgo::assignkbestsd(costMatrix, 5, 0.01, 100);
	 *
     *       for (size_t i = 0; i < assignments.size(); ++i)
     *       {
     *           std::cout << "assignments[" << i << "] : " << std::endl
     *                   << assignments.at(i) << std::endl;
     *           std::cout << "cost[" << i << "]: " << cost.at(i) << std::endl;
     *           std::cout << "solutionGap[" << i << "]: " << solutionGap.at(i) << std::endl;
     *       }
     *       return 0;
     *   }
     * \endcode
     * 
     * @cite [1] Popp, R. L., Pattipati, K., and Bar-Shalom, Y. (2001). M-best S-D
     * Assignment Algorithm with Application to Multitarget Tracking. IEEE
     * Transactions on Aerospace and Electronic Systems, 37(1), 22-39.
     * @cite [2] Deb, S., Yeddanapudi, M., Pattipati, K., & Bar-Shalom, Y. (1997).
     * A generalized SD assignment algorithm for multisensor-multitarget
     * state estimation. IEEE Transactions on Aerospace and Electronic
     * Systems, 33(2), 523-538.
     */
    std::tuple<std::vector<xt::xarray<size_t>>,
               std::vector<double>,
               std::vector<double>>
    assignkbestsd(xt::xarray<double> costMatrix,
                  size_t numSolutions = 1,
                  double desiredGap = 0.01,
                  size_t maxIterations = 100,
                  assign2dFcn_t algFcn = &assignauction);
}
#endif // !ASSIGN_K_BEST_SD_H
