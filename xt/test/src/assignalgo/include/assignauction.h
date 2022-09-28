#ifndef ASSIGN_AUCTION_H
#define ASSIGN_AUCTION_H
#include <xtensor/xarray.hpp>
#include <tuple>
namespace assignalgo
{
	/**
	 * @brief assigns rows to columns based on the COSTMATRIX using 
	 * the forward/reverse Auction assignment algorithm.
	 * 
	 * ASSIGNAUCTION assigns rows to columns based on the COSTMATRIX using 
	 * the forward/reverse Auction assignment algorithm which finds a 
	 * sub-optimal solution to the global nearest neighbor (GNN) assignment problem, 
	 * where each column is assigned to a row in a way that minimizes the total cost, 
	 * assuming their cost is less than the COSTOFNONASSIGNMENT parameter, which serves
     * as a gating parameter.
     * 
     * ASSIGNAUCTION is faster than ASSIGNMUNKRES for larger GNN assignments,
     * such as cases where there are more than 50 rows and columns in the
     * COSTMATRIX.
     * 
     * This assignment algorithm is useful, for example, in assigning tracks
     * to detections. This can be done by setting COSTMATRIX(i, j) to the cost
     * of assigning j-th detection to i-th track. The lower the cost, the more
     * likely the assignment is to be made.
	 * 
	 * 
     * \code .cpp
     * 	#include "assignalgo/include/assignauction.h"
	 *	#include <xtensor/xio.hpp>
	 *	#include <xtensor/xnorm.hpp>
	 *	#include <xtensor/xview.hpp>
	 *	using namespace xt::placeholders;
	 *	int main(int argc, char const *argv[])
	 *	{
	 *		xt::xarray<double> tracks = {{1, 1}, {2, 2}};
	 *		xt::xarray<double> dets = {{1.1, 1.1}, {2.1, 2.1}, {1.5, 3}};
	 *
	 *		xt::xarray<double> costMatrix = xt::empty<double>({tracks.shape(0), dets.shape(0)});
	 *		for (size_t i : xt::view(xt::arange<size_t>(tracks.shape(0)), xt::range(_, _, -1)))
	 *		{
	 *			xt::xarray<double> delta = dets - xt::row(tracks, i);
	 *			xt::row(costMatrix, i) = xt::norm_l2(delta, {1});
	 *		}
	 *		double costOfNonAssignment = 0.2;
	 *		xt::xarray<size_t> assignments, unassignedTracks, unassignedDetections;
	 *
	 *		std::tie(assignments, unassignedTracks, unassignedDetections) =
	 *				assignalgo::assignauction(costMatrix, costOfNonAssignment);
	 *
	 *		std::cout << costMatrix << std::endl;			  // {{ 0.141421,  1.555635,  2.061553}, { 1.272792, 0.141421, 1.118034}}
	 *		std::cout << assignments << std::endl;			  // {{0, 0}, {1, 1}}
	 *		std::cout << unassignedTracks << std::endl;		  // {}
	 *		std::cout << unassignedDetections << std::endl;   // {2}
	 *
	 *		return 0;
	 *	}
     * \endcode
	 *      
	 * @param COSTMATRIX is an M-by-N matrix, where M is the number of objects to
     * assign, e.g., tracks, and N is the number of objects to be assigned,
     * e.g., detections. 
	 * 
	 * @param COSTOFNONASSIGNMENT is a scalar, which represents the
     * cost of leaving unassigned objects. Higher COSTOFNONASSIGNMENT
     * corresponds to the higher likelihood that every existing object will be
     * assigned.
     * 
     * @return (ASSIGNMENTS, UNASSIGNEDROWS, UNASSIGNEDCOLUMNS) three-element tuple. 
	 * All three element of tuple are of the type xt::xarray<size_t>.
	 * 
     * The three elements of tuple are 
     * 
     * 1. ASSIGNMENTS is an L - by - 2 matrix of index pairs of rows and
	 * corresponding columns, where L is the number of pairs. The first column
     * in the ASSIGNMENTS matrix contains the row indices and the second
     * column contains the corresponding column indices. For example, in
     * assigning tracks to detections, the first column will represent the
     * tracks while the second is the corresponding detections.
     * 
     * 2. UNASSIGNEDROWS is a P - element vector, where P is the number of
     * unassigned rows. Each element is an index of a row to which no columns
     * were assigned. For example, in the aforementioned assignment of tracks
     * to detections, this vector will be of unassigned tracks.
     *
     * 3. UNASSIGNEDCOLUMNS is a Q - element column vector, where Q is the number
     * of unassigned columns. Each element is an index of a column that was
     * not assigned to any rows.For example, in the aforementioned assignment
     * of tracks to detections, this vector will be of unassigned detections.
	 * 	   
	 * @cite [1] D. Bertsekas and D. Castanon, A forward/reverse auction algorithm
	 * for asymmetric assignment problems, 1993
	 * @cite [2] D. Bertsekas, Linear network optimization: algorithms and codes,
	 * MIT Press, 1991
	 */
	std::tuple<xt::xarray<size_t>,
			   xt::xarray<size_t>,
			   xt::xarray<size_t>>
	assignauction(xt::xarray<double> costMatrix,
				  double costOfNonAssignment);
}
#endif // !ASSIGN_AUCTION_H
