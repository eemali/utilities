#include "../../include/assignauction.h"
#include "../../include/assignjv.h"
#include "../../include/assignmunkres.h"
#include "../../include/assignmunkres_fast.h"
#include <xtensor/xio.hpp>
#include <xtensor/xnorm.hpp>
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
int main(int argc, char const *argv[])
{
#if 0
	xt::xarray<double> tracks = { {1, 1}, {2, 2} };
	xt::xarray<double> dets = { {1.1, 1.1}, {2.1, 2.1}, {1.5, 3} };

	xt::xarray<double> costMatrix = xt::empty<double>({ tracks.shape(0), dets.shape(0) });
	for (size_t i : xt::view(xt::arange<size_t>(tracks.shape(0)), xt::range(_, _, -1)))
	{
		xt::xarray<double> delta = dets - xt::row(tracks, i);
		xt::row(costMatrix, i) = xt::norm_l2(delta, { 1 });

	}
#else
	xt::xarray<double> costMatrix = {{0.913337361501670, 0.0781755287531837, 0.774910464711502, 0.259870402850654},
									 {0.152378018969223, 0.442678269775446, 0.817303220653433, 0.800068480224308},
									 {0.825816977489547, 0.106652770180584, 0.868694705363510, 0.431413827463545},
									 {0.538342435260057, 0.961898080855054, 0.0844358455109103, 0.910647594429523},
									 {0.996134716626886, 0.00463422413406744, 0.399782649098897, 0.181847028302853}};
#endif // 0
	double costOfNonAssignment = 0.2;
	xt::xarray<size_t> assignments, unassignedTracks, unassignedDetections;
	// assignauction
	{
		std::tie(assignments, unassignedTracks, unassignedDetections) =
			assignalgo::assignauction(costMatrix, costOfNonAssignment);

		std::cout << costMatrix << std::endl;			// {{ 0.141421,  1.555635,  2.061553}, { 1.272792, 0.141421, 1.118034}}
		std::cout << assignments << std::endl;			// {{0, 0}, {1, 1}}
		std::cout << unassignedTracks << std::endl;		// {}
		std::cout << unassignedDetections << std::endl; // {2}
	}
	// assignjv
	{
		std::tie(assignments, unassignedTracks, unassignedDetections) =
			assignalgo::assignjv(costMatrix, costOfNonAssignment);

		std::cout << costMatrix << std::endl;			// {{ 0.141421,  1.555635,  2.061553}, { 1.272792, 0.141421, 1.118034}}
		std::cout << assignments << std::endl;			// {{0, 0}, {1, 1}}
		std::cout << unassignedTracks << std::endl;		// {}
		std::cout << unassignedDetections << std::endl; // {2}
	}
	// assignmunkres
	{
		std::tie(assignments, unassignedTracks, unassignedDetections) =
			assignalgo::assignmunkres(costMatrix, costOfNonAssignment);

		std::cout << costMatrix << std::endl;			// {{ 0.141421,  1.555635,  2.061553}, { 1.272792, 0.141421, 1.118034}}
		std::cout << assignments << std::endl;			// {{0, 0}, {1, 1}}
		std::cout << unassignedTracks << std::endl;		// {}
		std::cout << unassignedDetections << std::endl; // {2}
	}
	// assignmunkres_fast
	{
		std::tie(assignments, unassignedTracks, unassignedDetections) =
			assignalgo::assignmunkres_fast(costMatrix, costOfNonAssignment);

		std::cout << costMatrix << std::endl;			// {{ 0.141421,  1.555635,  2.061553}, { 1.272792, 0.141421, 1.118034}}
		std::cout << assignments << std::endl;			// {{0, 0}, {1, 1}}
		std::cout << unassignedTracks << std::endl;		// {}
		std::cout << unassignedDetections << std::endl; // {2}
	}
	return 0;
}