#ifndef LAP_PAD_FOR_UNASSIGNED_ROWS_AND_COLUMNS_H
#define LAP_PAD_FOR_UNASSIGNED_ROWS_AND_COLUMNS_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    /**
     * @brief Pads cost matrix to allow for non-assignment solutions
     * 
     * Pads COSTMATRIX to allow for solutions where rows and columns may not
     * be assigned.
     * 
     * @param COSTMATRIX is an M-by-N matrix, where each element defines the cost of
     * assigning column n to row m is represented as COSTMATRIX(m,n). Larger
     * assignment costs mean that the assignment is less likely to selected by
     * an algorithm as it seeks to minimize the overall cost of assignment of
     * all columns to rows.
     * 
     * @param COSTOFNONASSIGNMENT is a scalar value representing the cost of not
     * assigning a row or a column.
     * 
     * @return COSTMATRIXOUT is an L-by-L matrix, where L = M+N. The returned cost
     * matrix is of the form:
     * 
     *                                     | x      Inf     Inf     ...     Inf
     *                                     | Inf    x       Inf     ...     Inf
     *            COSTMATRIX               | Inf    Inf      x      ...      :
     *                                     |  :      :       :       x       :
     *                                     | Inf    Inf     Inf     ...     x
     * ------------------------------------+-----------------------------------
     * x      Inf     Inf     ...     Inf  |
     * Inf    x       Inf     ...     Inf  |
     * Inf    Inf      x      ...      :   |           ZEROS(N,M)
     * Inf    Inf     Inf     ...     x    |
     *      
     */
    xt::xarray<double> lapPadForUnassignedRowsAndColumns(xt::xarray<double> costMatrix,
                                                           double costOfNonAssignment);
}
#endif // !LAP_PAD_FOR_UNASSIGNED_ROWS_AND_COLUMNS_H