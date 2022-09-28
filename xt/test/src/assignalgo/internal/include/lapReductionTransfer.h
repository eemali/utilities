#ifndef LAP_REDUCTION_TRANSFER
#define LAP_REDUCTION_TRANSFER
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	/**
	 * @brief Exchanges the reduction value between columns and rows
	 * 
	 * Updates the column reduction values, COLREDUX, in the current step of
	 * the assignment solution by transferring the row reduction values to
	 * column reduction values. Reduction transfer  is a common operation in
	 * solutions to the Linear Assignment Problem (LAP) and typically follows
	 * column reduction to update column reduction values based on the number
	 * of row assignments that occurred in the column reduction.
	 * 
	 * @param COSTMATRIX is an M-by-N matrix, where each element defines the cost of
	 * assigning column n to row m is represented as COSTMATRIX(m,n). Larger
	 * assignment costs mean that the assignment is less likely to selected by
	 * the algorithm as it seeks to minimize the overall cost of assignment of
	 * all columns to rows.
	 * 
	 * @param ROWSOLN is an M element vector, with each element set to the
	 * column assigned to the corresponding row. When no column is assigned,
	 * the element is set to NaN.
	 * 
	 * @param COLREDUX is an N element ector of the column reduction values
	 * corresponding to the assignment solution.
	 * 
	 * @param ROWASSIGNEDCNT is an M element vector indicating the number of times a
	 * row was assigned to a column in the current step of the assignment
	 * solution.
	 * 
	 * @cite [1] R. Jonker and A. Volgenant, A shortest augmenting path algorithm
	 * for dense and sparse linear assignment problems, 1987, Computing 38, 4
	 * (November 1987), 325-340.
	 * @cite [2] A. Volgenant, Linear and semi-assignment problems: a core oriented
	 * approach, 1996, Computers & Operations Research 23, 10 (October 1996),
	 * 917-932.
	 */
	xt::xarray<double> lapReductionTransfer(const xt::xarray<double> &costMatrix,
											const xt::xarray<double> &rowSoln,
											xt::xarray<double> colRedux,
											const xt::xarray<size_t> &rowAssignedCnt);
}
#endif // !LAP_REDUCTION_TRANSFER
