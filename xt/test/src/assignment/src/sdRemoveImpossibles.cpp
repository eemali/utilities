#include "../include/sdRemoveImpossibles.h"
#include <xtensor/xadapt.hpp>
#include <xtensor/xindex_view.hpp>
namespace assignalgo
{
	void sdRemoveImpossibles(xt::xarray<double> &costMatrix)
	{
		xt::xarray<double> temporaryMatrix = costMatrix;

		xt::xarray<size_t> dimSize = xt::adapt(costMatrix.shape());

		size_t numDims = dimSize.size();

		xt::xstrided_slice_vector oneArgs(numDims, 0);
		std::vector<size_t> oneShape(numDims, 1);

		xt::xstrided_slice_vector thisArg;
		std::vector<size_t> thisShape;
		for (size_t i = 0; i < numDims; ++i)
		{
			thisArg = oneArgs;
			thisShape = oneShape;
			thisArg.at(i) = xt::all();
			thisShape.at(i) = dimSize.at(i);
			temporaryMatrix = temporaryMatrix - xt::reshape_view(xt::strided_view(temporaryMatrix, thisArg), thisShape);
		}
		xt::filtration(costMatrix, temporaryMatrix > 0) = std::numeric_limits<double>::infinity();
	}
}