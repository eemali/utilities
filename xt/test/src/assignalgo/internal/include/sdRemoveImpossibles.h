#ifndef SD_REMOVE_IMPOSSIBLES_H
#define SD_REMOVE_IMPOSSIBLES_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
	/**
	 * @brief removes impossibles from a
	 * costMatrix. It gates the impossible assignments by setting their values 
	 * to Inf.	  
	 */
	xt::xarray<double> sdRemoveImpossibles(xt::xarray<double> costMatrix);
}
#endif // !SD_REMOVE_IMPOSSIBLES_H
