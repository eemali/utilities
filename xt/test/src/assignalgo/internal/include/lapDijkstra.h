#ifndef LAP_DIJKSTRA_H
#define LAP_DIJKSTRA_H
#include "assigntypedef.h"
namespace assignalgo
{
	/**
	 * @brief Solution to the Linear Assignment Problem (LAP) 
	 * using Dijkstra's shortest path algorithm
	 * 
	 * Assigns rows to columns based on the COSTMATRIX using Dijkstra's
	 * shortest path assignment algorithm, where each column is assigned to a
	 * row in a way that minimizes the total cost.
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
	 * @cite [1] E. Dijkstra, A note on two problems in connexion with graphs,
	 * Numerische Mathematik 1, 1959, 269-271.
	 */
	std::tuple<xt::xarray<double>,
			   xt::xarray<double>,
			   xt::xarray<double>>
	lapDijkstra(const xt::xarray<double> &costMatrix,
				xt::xarray<double> rowSoln = xt::xarray<double>(),
				xt::xarray<double> colSoln = xt::xarray<double>(),
				xt::xarray<double> colRedux = xt::xarray<double>());
}
#endif // !LAP_DIJKSTRA_H
