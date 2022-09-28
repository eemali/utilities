#ifndef LAP_DIJKSTRA_H
#define LAP_DIJKSTRA_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	void lapDijkstra(xt::xarray<double> &costMatrix,
					 xt::xarray<double> &rowSoln,
					 xt::xarray<double> &colSoln,
					 xt::xarray<double> &colRedux);
}
#endif // !LAP_DIJKSTRA_H
