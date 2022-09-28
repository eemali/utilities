#ifndef LAP_REMOVE_IMPOSSIBLES_H
#define LAP_REMOVE_IMPOSSIBLES_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    /**
     * @brief Removes impossible assignments from COSTMATRIX
     * 
     * Removes rows and columns from COSTMATRIX where all values in the row or
     * column are set to Inf. No assignment is possible for these rows or
     * columns and they will be unassigned in the final assignment solution.
	 
     * @param COSTMATRIX is an M-by-N matrix, where each element defines the cost of
     * assigning column n to row m is represented as COSTMATRIX(m,n). Larger
     * assignment costs mean that the assignment is less likely to selected by
     * the algorithm as it seeks to minimize the overall cost of assignment of
     * all columns to rows.
	 * 
	 * @param COSTOFNONASSIGNMENT is a scalar, which represents the
     * cost of leaving unassigned objects. When a value in the cost matrix is greater-than or
     * equal-to twice the cost of non-assignment, that row and column in the
     * cost matrix will not be assigned.
     * 
     *  @return (COSTMATRIXOUT, ROWIDX, COLIDX, UNASSIGNEDROWS, UNASSIGNEDCOLS)
     * five-element tuple.
     * 
     * The five elements of tuple are
     * 
     * 1. COSTMATRIXOUT is a P-by-Q matrix , where rows and columns in COSTMATRIX
     * where assignment was not possible have been removed. Its datatype is 
     * xt::xarray<double>.
     * 
     * 2. ROWIDX is a output vector of the row indices in COSTMATRIX which are
     * returned in COSTMATRIXOUT. Its datatype is xt::xarray<size_t>.
     * 
     * 3. COLIDX is a output vector of column indices in COSTMATRIX which are
     * returned in COSTMATRIXOUT. Its datatype is xt::xarray<size_t>.
     * 
     * 4. UNASSIGNEDROWS is a output vector of the indices of rows in COSTMATRIX
     * which were removed because assignment was not possible. Its datatype is 
     * xt::xarray<size_t>.
     * 
     * 5. UNASSIGNEDCOLS is a output vector of the indices of columns in COSTMATRIX
     * which were removed because assignment was not possible. Its datatype is 
     * xt::xarray<size_t>.
     */
    std::tuple<xt::xarray<double>,
               xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    lapRemoveImpossibles(xt::xarray<double> costMatrix,
                         double costOfNonAssignment);
}
#endif // !LAP_REMOVE_IMPOSSIBLES_H