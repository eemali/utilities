#include "../include/kbestEnforceConstraints.h"
#include <xtensor/xview.hpp>
#include <xtensor/xstrided_view.hpp>
namespace assignalgo
{
    static void removeTuple(xt::xarray<double> &costMatrix,
                            const xt::xarray<size_t> &tuples);
    static void enforceTuple(xt::xarray<double> &costMatrix,
                             const xt::xarray<size_t> &tuples);

    xt::xarray<double> kbestEnforceConstraints(const xt::xarray<double> &costMatrix,
                                               const xt::xarray<size_t> &tuplesToRemove,
                                               const xt::xarray<size_t> &tuplesToEnforce)
    {
        // kbestEnforceConstraints - Enforce the constraints on a costMatrix.
        // This function enforces the constraints on a costMatrix. tuplesToRemove is
        // a N-by-2 list, which defines the assignments which should not be a part of
        // the assignment. tuplesToEnforce is a N-by-2 list, which defines tuples
        // which should be a part of the assignment.
        xt::xarray<double> constrainedCost = costMatrix;
        removeTuple(constrainedCost, tuplesToRemove);
        enforceTuple(constrainedCost, tuplesToEnforce);
        return constrainedCost;
    }

    static void removeTuple(xt::xarray<double> &costMatrix,
                            const xt::xarray<size_t> &tuples)
    {
        // This function modifies the cost matrix such that the input assignment,
        // tuples, does not appear in the solution to costMatrix.
        // To remove the tuple from the list, it's corresponding assignment value is
        // set to Inf.
        double inf = std::numeric_limits<double>::infinity();

        xt::xarray<size_t> idx;
        for (size_t i = 0; i < tuples.shape(0); ++i)
        {
            idx = xt::row(tuples, i);
            xt::strided_view(costMatrix, xt::xstrided_slice_vector(idx.cbegin(), idx.cend())) = inf;
        }
    }

    static void enforceTuple(xt::xarray<double> &costMatrix,
                             const xt::xarray<size_t> &tuples)
    {
        // This function modifies the costMatrix so that the input assignment,
        // tuples, is forced in the solution of the assignment problem. To enforce a
        // tuple, all feasible assignments are set to Inf except the enforced one.
        // For example, to enforce assignment (2,3) set costMatrix(:,3) = Inf and
        // costMatrix(2,:) = Inf and retain costMatrix(2,3) to it's original value.
		double inf = std::numeric_limits<double>::infinity();

		size_t nTuplesRows = tuples.shape(0);
		size_t nTuplesCols = tuples.shape(1);
		size_t numDims = costMatrix.shape().size();
		xt::xstrided_slice_vector allArgs(numDims, xt::all()), indices, tuplesInCell;
		xt::xarray<double> tempCost;
		xt::xarray<size_t> idx;
		size_t tuplse_ij;
		for (size_t i = 0; i < nTuplesRows; ++i)
		{
			idx = xt::row(tuples, i);
			tuplesInCell =  xt::xstrided_slice_vector(idx.cbegin(), idx.cend());
			tempCost = xt::strided_view(costMatrix, tuplesInCell);
			for (size_t j = 0; j < nTuplesCols; ++j)
			{
				tuplse_ij = tuples.at(i, j);
				if (!(0 == tuplse_ij && numDims > 2))
				{
					indices = allArgs;
					indices[j] = tuplse_ij;
					xt::strided_view(costMatrix, indices) = inf;
				}
			}
			// Store back the only permissive cost.
			xt::strided_view(costMatrix, tuplesInCell) = tempCost;
		}
    }
}