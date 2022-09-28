#ifndef SD_REMOVE_IMPOSSIBLES_H
#define SD_REMOVE_IMPOSSIBLES_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	void sdRemoveImpossibles(xt::xarray<double> &costMatrix);
}
#endif // !SD_REMOVE_IMPOSSIBLES_H
