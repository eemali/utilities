#ifndef K_BEST_REMOVE_UNASSIGNED_H
#define K_BEST_REMOVE_UNASSIGNED_H
#include <xtensor/xarray.hpp>
#include <tuple>
namespace assignalgo
{
    /**
     * @brief remove unassigned rows and columns from the assignment
     * solution of a padded cost matrix.
     * 
     * @param assignments is a P-by-2 list of solutions to the padded cost matrix.
     * @param costSize is a 2-element vector defining the size of original cost matrix.
     * 
     * @return (assignment,unassignedRows,unassignedCols) three-element tuple. 
	 * All three element of tuple are of the type xt::xarray<size_t>.
	 * 
     * The three-element of tuple are 
     * 1. assignments - a N-by-2 list of solutions to the original cost matrix.
     * 2. unassignedRows - a list of unassigned rows 
     * 3. unassignedCols - a list of unassigned columns.
     */
    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    kbestRemoveUnassigned(xt::xarray<size_t> assignment,
                          const xt::xarray<size_t> &costSize);
}
#endif // !K_BEST_REMOVE_UNASSIGNED_H
