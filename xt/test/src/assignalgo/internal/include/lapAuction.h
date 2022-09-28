#ifndef LAP_AUCTION_H
#define LAP_AUCTION_H
#include "assigntypedef.h"
namespace assignalgo
{
    /**
     * @brief Solution to the Linear Assignment Problem (LAP) 
     * using forward Auction
     * 
     * Assigns rows to columns based on the COSTMATRIX using the
     * forward Auction assignment algorithm, where each column is
     * assigned to a row in a way that minimizes the total cost.
     * 
     * @param COSTMATRIX is an M-by-N matrix, where each element defines the cost of
     * assigning column n to row m is represented as COSTMATRIX(m,n). Larger
     * assignment costs mean that the assignment is less likely to selected by
     * the algorithm as it seeks to minimize the overall cost of assignment of
     * all columns to rows.
     * 
     * @param ROWSOLN Algorithm can use a partial solution to the LAP by passing in the
     * ROWSOLN from a previous partial solution. If not
     * provided, the algorithm assumes no partial solution has been provided.
     * 
     * @param COLSOLN Algorithm can use a partial solution to the LAP by passing in the
     * COLSOLN from a previous partial solution. If not
     * provided, the algorithm assumes no partial solution has been provided.
     * 
     * @param COLREDUX Algorithm can use a partial solution to the LAP by passing in the
     * COLREDUX from a previous partial solution. If not
     * provided, the algorithm assumes no partial solution has been provided.
     * 
     * @param MAXAUCTIONS The number of auction cycles can be limited by setting MAXAUCTIONS to a
     * positive integer. If not provided, the algorithm iterates until the
     * optimal solution which minimizes the total assignment cost is found. By
     * setting MAXAUCTIONS, an optimal solution is not guaranteed, and a
     * partial solution may be returned.
     * 
     * @param HASEPSILONSCALING Enabling epsilon-scaling by setting HASEPSILONSCALING to true can speed
     * up the algorithm's convergence for some linear assignment problems.
     * 
     * @return (ROWSOLN, COLSOLN, COLREDUX) three-element tuple. 
	 * All three element of tuple are of the type xt::xarray<double>.
	 * 
     * The three elements of tuple are 
     *
     * 1. ROWSOLN is an M element vector, with each element set to the
     * column assigned to the corresponding row. When no column is assigned,
     * the element is set to NaN.
     * 
     * 2. COLSOLN is an N element vector, with each element set to the row
     * assigned to the corresponding column. When no row is assigned, the
     * element is set to NaN.
     * 
     * 3. COLREDUX is an N element row vector of the column reduction values
     * corresponding to the returned assignment solution.
     * 
     * @cite [1] D. Bertsekas, Auction algorithms, Encyclopedia of Optimization,
     * Kluwer, 2001
     * @cite [2] D. Bertsekas, Linear network optimization: algorithms and codes,
     * MIT Press, 1991
     */
    std::tuple<xt::xarray<double>,
               xt::xarray<double>,
               xt::xarray<double>>
    lapAuction(const xt::xarray<double> &costMatrix,
               xt::xarray<double> rowSoln = xt::xarray<double>(),
               xt::xarray<double> colSoln = xt::xarray<double>(),
               xt::xarray<double> colRedux = xt::xarray<double>(),
               double maxAuctions = std::numeric_limits<double>::infinity(),
               bool hasEpsilonScaling = true);
}
#endif // !LAP_AUCTION_H
