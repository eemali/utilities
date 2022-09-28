#include "../include/xsortrows.h"
#include <xtensor/xview.hpp>
#include <xtensor/xsort.hpp>
namespace xt
{
    xt::xarray<size_t> sortrows(const xt::xarray<size_t> &A)
    {
		if (2 != A.dimension())
		{
			XTENSOR_THROW(std::runtime_error, "Input must be 2D matrix");
		}
        size_t nRows = A.shape(0);
        size_t nCols = A.shape(1);
        xt::xarray<size_t> I = xt::arange<size_t>(nRows);
        xt::xarray<size_t> ind;
        for (int k = nCols - 1; k >= 0; k--)
        {
            ind = xt::argsort(xt::eval(xt::view(A, xt::keep(I), k)));
            I = xt::view(I, xt::keep(ind));
        }
        xt::xarray<size_t> B = xt::view(A, xt::keep(I));
        return B;
    }
}