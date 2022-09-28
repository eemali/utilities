#ifndef LAP_COLUMN_REDUCTION_H
#define LAP_COLUMN_REDUCTION_H
#include "assigntypedef.h"
namespace assignalgo
{
	/**
	 * @brief Performs column reduction on the current assignment solution
	 * 
	 * Performs column reduction on the current partial assignment solution.
	 * Column reduction is a common operation in solutions to the Linear
	 * Assignment Problem (LAP) and is the first step in the Jonker-Volgenant
	 * assignment.
	 * 
	 * @param COSTMATRIX is an M-by-N matrix, where each element defines the cost of
	 * assigning column n to row m is represented as COSTMATRIX(m,n). Larger
	 * assignment costs mean that the assignment is less likely to selected by
	 * the algorithm as it seeks to minimize the overall cost of assignment of
	 * all columns to rows.
	 * 
	 * @param ROWSOLN is an M element column vector, with each element set to the
	 * column assigned to the corresponding row. When no column is assigned,
	 * the element is set to NaN.
	 * 
	 * @param COLSOLN is an N element row vector, with each element set to the row
	 * assigned to the corresponding column. When no row is assigned, the
	 * element is set to NaN.
	 * 
	 * @param COLREDUX is an N element row vector of the column reduction values
	 * corresponding to the assignment solution.
	 * 
	 * @return ROWASSIGNEDCNT is an M element vector indicating the number of times a
	 * row was assigned to a column in the current step of the assignment
	 * solution.
	 * 
	 * @cite [1] R. Jonker and A. Volgenant, A shortest augmenting path algorithm
	 * for dense and sparse linear assignment problems, 1987, Computing 38, 4
	 * (November 1987), 325-340.
	 * 
	 * @cite [2] A. Volgenant, Linear and semi-assignment problems: a core oriented
	 * approach, 1996, Computers & Operations Research 23, 10 (October 1996),
	 * 917-932. 
	 */
	std::tuple<xt::xarray<double>,
			   xt::xarray<double>,
			   xt::xarray<double>,
			   xt::xarray<size_t>>
	lapColumnReduction(const xt::xarray<double> &costMatrix,
					   xt::xarray<double> rowSoln = xt::xarray<double>(),
					   xt::xarray<double> colSoln = xt::xarray<double>(),
					   xt::xarray<double> colRedux = xt::xarray<double>());
}
#endif // !LAP_COLUMN_REDUCTION_H
