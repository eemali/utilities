#include "../include/lapColumnReduction.h"
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xsort.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
	static bool isempty(const xt::xarray<double> &arr);

	void lapColumnReduction(xt::xarray<double> &costMatrix,
							xt::xarray<double> &rowSoln,
							xt::xarray<double> &colSoln,
							xt::xarray<double> &colRedux,
							xt::xarray<size_t> &rowAssignedCnt)
	{
		// For square cost matrices (nRows == nColumns), the first step in JV
		// Assignment assigns columns to unassigned rows with the least cost
		size_t nRow = costMatrix.shape(0);
		size_t nCol = costMatrix.shape(1);

		double nan = std::numeric_limits<double>::quiet_NaN();
		if (isempty(rowSoln))
		{
			rowSoln = xt::xarray<double>({nRow}, nan);
		}
		if (isempty(colSoln))
		{
			colSoln = xt::xarray<double>({nCol}, nan);
		}
		if (isempty(colRedux))
		{
			colRedux = xt::zeros<double>({nCol});
		}
		rowAssignedCnt = xt::zeros<size_t>({nRow});

		size_t rowMin;
		double costMin, colAssigned;
		// Starting with the last column, find row in each column with the minimum
		// cost. If the min-valued row is not already assigned, assign that row to
		// the column
		for (size_t col : xt::view(xt::arange<size_t>(nCol), xt::range(_, _, -1)))
		{
			// Find min-valued row
			costMin = xt::nanmin(xt::col(costMatrix, col))();
			rowMin = xt::flatten_indices(xt::argwhere(xt::equal(xt::col(costMatrix, col), costMin))).at(0);

			// Update column reduction
			colRedux.at(col) = costMin;

			// Update the number of times this row was the minimum
			rowAssignedCnt.at(rowMin) = rowAssignedCnt.at(rowMin) + 1;

			// Assign row to column, if not already assigned
			colAssigned = rowSoln.at(rowMin);

			if (std::isnan(colAssigned))
			{
				rowSoln.at(rowMin) = col;
				colSoln.at(col) = rowMin;
			}
			else if (costMin < colRedux.at(colAssigned))
			{
				// If the column this row was previously assigned to had a higher
				// cost than this new column, then reassign it
				rowSoln.at(rowMin) = col;
				colSoln.at(col) = rowMin;
				colSoln.at(colAssigned) = nan;
			}
		}
	}

	static bool isempty(const xt::xarray<double> &arr)
	{
		return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
	}
}