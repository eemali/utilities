#ifndef ASSIGN_TOMHT_H
#define ASSIGN_TOMHT_H
#include <xtensor/xarray.hpp>
#include <tuple>
namespace assignalgo
{

    /**
     * @brief Track-oriented multi-hypotheses tracking (TOMHT) assignment
     * 
     * It returns a list of assignments of detections to tracks, a list of
     * unassigned tracks, and a list of unassigned detections. 
     * 
     * @param COSTMATRIX must be an M-by-N real matrix, where M is 
     * the number of tracks and N is the number of detections.
     * 
     * @param COSTTHRESHOLD must be a 3-element finite, real, vector 
     * [C1Gate,C2Gate,C3Gate], where C1Gate <= C2Gate <= C3Gate.
     * 
     * Let c = costMatrix(i,j). The following cases exist:
     * 
     * Case         | Track unassigned | Detection unassigned | Detection is assigned to track
     * -----------------------------------------------------------------------
     * c < C1       | No               | No                   | Yes
     * C1 <= c < C2 | Yes              | No                   | Yes
     * C2 <= c < C3 | Yes              | Yes                  | Yes
     * C3 <= c      | Yes              | Yes                  | No
     * 
     * @return (ASSIGNMENTS, UNASSIGNEDTRACKS, UNASSIGNEDDETECTIONS) three-element tuple. 
	 * All three element of tuple are of the type xt::xarray<size_t>.
	 * 
     * The three-element of tuple are 
     * 
     * 1. ASSIGNMENTS is a P-by-2 matrix, where P is the
     * number of possible assignments. The first column lists the row indices
     * in the costMatrix input, corresponding to tracks, and the second column
     * lists the column indices, corresponding to detections.
     * 
     * 2. UNASSIGNEDTRACKS is a Q-element vector, where Q is the
     * number of unassigned tracks. The values correspond to the row indices.
     * 
     * 3. UNASSIGNEDDETECTIONS is an R-element vector, where R is
     * the number of unassigned detections. The values correspond to the
     * column indices.
     * 
     * Notes:
     * 1. To allow each track to be unassigned, use C1Gate = 0.
     * 2. To allow each detection to be unassigned, use C2Gate = 0.
     * 
     * \code{cpp}
     * #include "../../include/assignTOMHT.h"
     * #include <xtensor/xio.hpp>
     * int main(int argc, char const *argv[])
     * {
     *      double Inf = std::numeric_limits<double>::infinity();
     *      xt::xarray<double> costMatrix = {{4, 9, 200, Inf},
     *                                       {300, 12, 28, Inf},
     *                                       {32, 100, 210, 1000}};
     *      xt::xarray<double> costThreshold = {5, 10, 30};
     * 
     *      xt::xarray<size_t> assignments, unassignedTracks, unassignedDetections;
     * 
     *      std::tie(assignments, unassignedTracks, unassignedDetections) =
     *           assignalgo::assignTOMHT(costMatrix, costThreshold);
     *
     *      std::cout << costMatrix << std::endl;           // {{4., 9., 200., inf}, {300., 12., 28., inf}, {32., 100., 210., 1000.}}
     *      std::cout << costThreshold << std::endl;        // {5., 10., 30.}
     *      std::cout << assignments << std::endl;          // {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
     *      std::cout << unassignedTracks << std::endl;     // {1, 2}
     *      std::cout << unassignedDetections << std::endl; // {2, 3}
     *      return 0;
     * }
     * \endcode
     * 
     * @cite [1] J.R. Werthmann, "A Step-by-Step Description of a Computationally
     * Efficient Version of Multiple Hypothesis Tracking", SPIE Vol. 1698
     * Signal and Processing of Small Targets, pp 288-300, 1992.
     */
    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>>
    assignTOMHT(const xt::xarray<double> &costMatrix,
                const xt::xarray<double> &costThreshold);
}
#endif // !ASSIGN_TOMHT_H
