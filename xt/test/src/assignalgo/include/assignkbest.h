#ifndef ASSIGN_K_BEST_H
#define ASSIGN_K_BEST_H
#include "assignauction.h"
#include "../internal/include/assigntypedef.h"
#include <vector>
namespace assignalgo
{
    /**
     * @brief K-best solutions to the assignment problem
     * 
     * finds the best global nearest-neighbor solution that minimize the total cost of
     * assignment. It uses the Munkres algorithm to minimize the cost of assignment.
     * 
     * @param costMatrix is an M-by-N matrix, where M is the number of tracks, and N
     * is the number of detections. costMatrix(i,j) is the cost of assigning
     * j-th detection to i-th track. The lower the cost, the more likely the
     * assignment is to be made.
     * 
     * @param costOfNonAssignment is a scalar, which represents the cost of a track
     * or a detection remaining unassigned.  Higher costOfNonAssignment
     * corresponds to the higher likelihood that every existing track will be
     * assigned a detection.
     * 
     * @param numSolutions number of best global nearest-neighbor solutions that 
     * minimize the total cost of assignment. 
     * It uses Murty's algorithm to find numSolutions-1 solutions, in
     * addition to the best solution.
     * 
     * @param algFcn an algorithm used for finding the assignment
     * The choices of algorithm are: &assignauction, &assignjv, &assignmunkres
     * 
     * @return (assignments,unassignedRows,unassignedCols,cost) four-element tuple
     * The first three element of tuple are of the type std::vector<xt::xarray<size_t>>,
     * and last element of tuple is of the type std::vector<double>.
     * 
     *  The four-element of tuple are
     * 
     * 1. assignments is a k-element vector. Each cell element is an Li-by-2
     * matrix of index pairs of tracks and corresponding detections, where Li
     * is the number of pairs in the ith solution. The first column contains 
     * the row indices and the second column contains the corresponding 
     * column indices.
     * 
     * 2. unassignedRows is a k-element cell array. Each cell element is a
     * Pi-element, where Pi = M - Li is the number of unassigned rows. Each
     * element is an index of a row to which no columns were assigned.
     * 
     * 3. unassignedCols is a k-element cell array. Each cell element is a 
     * Qi-element vector, where Qi = N - Li is the number of unassigned 
     * columns. Each element is an index of a column that was not assigned to 
     * any rows. 
     * 
     * 4. cost is a k-element array. Each element is a scalar value summarizing
     * the total cost of the solution to the assignment problem.
     * 
     * \code{cpp}
     * #include "assignalgo/include/assignkbest.h"
     * #include <xtensor/xio.hpp>
     *   int main(int argc, char const *argv[])
     *   {
     *       double inf = std::numeric_limits<double>::infinity();
     *       xt::xarray<double> costMatrix = {{10, 5, 8, 9},
     *                                       {7, inf, 20, inf},
     *                                       {inf, 21, inf, inf},
     *                                       {inf, 15, 17, inf},
     *                                       {inf, inf, 16, 22}};
     *       double costOfNonAssignment = 100;
	 *
     *       std::vector<xt::xarray<size_t>> assignments, unassignedRows, unassignedColumns;
     *       std::vector<double> cost;
     *       size_t numSolutions = 5;
     *       std::tie(assignments, unassignedRows, unassignedColumns, cost) = assignalgo::assignkbest(costMatrix, costOfNonAssignment, numSolutions);
     *       for (size_t i = 0; i < assignments.size(); ++i)
     *       {
     *           std::cout << "assignments[" << i << "] : " << std::endl
     *                   << assignments.at(i) << std::endl;
     *           std::cout << "unassignedRows[" << i << "]: " << unassignedRows.at(i) << std::endl;
     *           std::cout << "unassignedColumns[" << i << "] : " << unassignedColumns.at(i) << std::endl;
     *           std::cout << "cost[" << i << "]: " << cost.at(i) << std::endl;
     *       }
     *       return 0;
     *   }
     * \endcode
     * 
     * @cite [1] Samuel Blackman and Robert Popoli, "Design and Analysis of Modern
     * Tracking Systems", Artech House, 1999.
     */
    std::tuple<std::vector<xt::xarray<size_t>>,
               std::vector<xt::xarray<size_t>>,
               std::vector<xt::xarray<size_t>>,
               std::vector<double>>
    assignkbest(xt::xarray<double> costMatrix,
                double costOfNonAssignment,
                size_t numSolutions = 1,
                assign2dFcn_t algFcn = &assignauction);
}
#endif // !ASSIGN_K_BEST_H
