#include "../include/lapReductionTransfer.h"
#include <xtensor/xview.hpp>
namespace assignalgo
{
	void lapReductionTransfer(xt::xarray<double> &costMatrix,
							  xt::xarray<double> &rowSoln,
							  xt::xarray<double> &colRedux,
							  xt::xarray<size_t> &rowAssignedCnt)
	{
		// For square cost matrices (nRows == nColumns), the second step in JV
		// Assignment enables the next step (Augmenting Row Reduction) by adjusting
		// the column reduction values to ensure that the costs across each row will
		// not include negative values

		// Update column reduction values for rows that were assigned only once
		xt::xarray<size_t> assignedOnceRows = xt::flatten_indices(xt::argwhere(!xt::isnan(rowSoln) & xt::equal(rowAssignedCnt, 1)));
		xt::xarray<double> rowCost;
		double costMin, colAssigned;

		double nan = std::numeric_limits<double>::quiet_NaN();
		for (size_t row : assignedOnceRows)
		{
			// Find minimum value for the column not assigned to this row
			rowCost = xt::row(costMatrix, row) - colRedux;
			colAssigned = rowSoln.at(row);
			rowCost.at(colAssigned) = nan;
			costMin = xt::nanmin(rowCost)();
			// Update the column reduction by this minimum value
			colRedux.at(colAssigned) = colRedux.at(colAssigned) - costMin;
		}
	}
}