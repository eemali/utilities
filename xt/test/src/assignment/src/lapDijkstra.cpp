#include "../include/lapDijkstra.h"
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xindex_view.hpp>
namespace assignalgo
{
	static bool isempty(const xt::xarray<double> &arr);

	void lapDijkstra(xt::xarray<double> &costMatrix,
					 xt::xarray<double> &rowSoln,
					 xt::xarray<double> &colSoln,
					 xt::xarray<double> &colRedux)
	{
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

		// Nothing to do?
		if (!xt::any(xt::isnan(rowSoln)))
		{
			return;
		}

		double distMin, costOld;
		size_t low, up, last, endOfPath, nMin, colMin, rowOld, rowAssigned, colAssigned;
		bool unassignedFound, noMinFound;
		xt::xarray<double> dist, costNew, edgeLen, distAdj;
		xt::xarray<size_t> prevRow, colList, tmp, k, kMin, colNew, colReady;
		xt::xarray<bool> isMin, isUnassignedCol, doRelax, isMinLen, isTerminal;

		xt::xarray<size_t> freeRows = xt::flatten_indices(xt::argwhere(xt::isnan(rowSoln)));
		for (size_t rowFree : freeRows)
		{
			// cost of assigning this row to each col
			dist = xt::row(costMatrix, rowFree) - colRedux;

			// Do any valid distances exist for this row?
			if (!xt::all(xt::isinf(dist)))
			{
				// Paths alternate between rows and cols, prevRow keeps track of each
				// column's previous row
				prevRow = xt::xarray<size_t>({nCol}, rowFree); // Set previous row in tree as this row

				// colList places columns in 1 of 3 queues. 'low' and 'up' partition
				// colList according to the table below. Its entries are indices into
				// colSoln
				//   [1:low-1]   : columns ready to have their assignment finalized
				//   [low:up-1]  : columns to be scanned for *current* minimum
				//   [up:n]      : columns to be scanned later for a *new* minimum
				low = 0;
				up = 0;
				colList = xt::arange<size_t>(nCol);
				last = 0;
				endOfPath = 0;

				unassignedFound = false;
				noMinFound = false;
				distMin = 0.0;

				while (!unassignedFound)
				{
					// If no min-valued columns exist, search for a new set
					if (low == up)
					{
						last = low;
						// Find all columns with min value and place them in the
						// 'current minimum' queue
						k = xt::arange<size_t>(up, nCol);
						// distMin = min(dist(colList(k)));
						distMin = xt::nanmin(xt::view(dist, xt::keep(xt::view(colList, xt::keep(k)))))();

						// Skip to the next row since there are no min-valued
						// columns left for this row
						if (std::isinf(distMin))
						{
							noMinFound = true;
							break;
						}

						isMin = xt::equal(xt::view(dist, xt::keep(xt::view(colList, xt::keep(k)))), distMin);
						kMin = xt::filter(k, isMin);

						// Check if any of the min-valued columns correspond to an
						// unassigned column, if so the alternating path is complete
						isUnassignedCol = xt::isnan(xt::view(colSoln, xt::keep(xt::view(colList, xt::keep(kMin)))));
						if (xt::any(isUnassignedCol))
						{
							endOfPath = xt::view(colList, xt::keep(xt::filter(kMin, isUnassignedCol)))(0);
							unassignedFound = true;
						}
						else
						{
							// Swap values to place in queue and avoid overwrite
							nMin = xt::sum(isMin)();

							tmp = xt::view(colList, xt::keep(k));
							xt::view(colList, xt::keep(xt::arange<size_t>(nMin) + up)) = xt::filter(tmp, isMin);
							up = up + nMin;
							xt::view(colList, xt::keep(xt::arange<size_t>(xt::sum(!isMin)()) + up)) = xt::filter(tmp, !isMin);
						}
					}

					if (!unassignedFound)
					{
						// Use a modified version of Dijkstra's algorithm to find the
						// path alternating between rows and columns with the least
						// total cost of assignment
						//
						// This maps into Dijkstra's algorithm as follows:
						//
						// - The source vertex is the new assignment: (rowFree, colMin)
						// - The distance of the source is its cost
						// - The neighboring vertices are the assignments of the
						//   currently assigned row (rowOld) with all of the
						//   non-min-valued columns (newCol)
						// - The distance of each neighbor is its cost
						// - The length of the edge connecting a neighbor to the source
						//   is the difference in cost between rowOld and rowFree
						//
						// The path terminates when an unassigned column is found

						colMin = colList.at(low);
						low = low + 1; // Search next min-valued column on next loop

						rowOld = (size_t)colSoln.at(colMin); // Currently assigned row
						costOld = costMatrix.at(rowOld, colMin) - colRedux.at(colMin);

						k = xt::arange<size_t>(up, nCol);
						colNew = xt::view(colList, xt::keep(k));
						costNew = xt::view(costMatrix, rowOld, xt::keep(colNew)) - xt::view(colRedux, xt::keep(colNew));

						// Compute edge lengths
						edgeLen = costNew - costOld;

						// Apply relaxation
						distAdj = edgeLen + distMin;
						doRelax = distAdj < xt::view(dist, xt::keep(colNew));
						// dist(colNew(doRelax)) = distAdj(doRelax);
						xt::view(dist, xt::keep(xt::filter(colNew, doRelax))) = xt::filter(distAdj, doRelax);
						// prevRow(colNew(doRelax)) = rowOld;
						xt::view(prevRow, xt::keep(xt::filter(colNew, doRelax))) = rowOld;

						// Check for terminal condition for the alternating path for any
						// of the relaxed columns
						isMinLen = xt::equal(distAdj, distMin) & doRelax;
						isTerminal = xt::isnan(xt::view(colSoln, xt::keep(colNew))) & isMinLen;

						if (xt::any(isTerminal))
						{
							endOfPath = xt::filter(colNew, isTerminal)[0];
							unassignedFound = true;
						}
						else if (xt::any(isMinLen))
						{
							// Terminal condition not met, add relaxed columns to be
							// scanned on next loop

							// Swap values to place in queue to avoid overwrite
							nMin = xt::sum(isMinLen)();

							tmp = xt::view(colList, xt::keep(k));
							xt::view(colList, xt::keep(xt::arange<size_t>(nMin) + up)) = xt::filter(tmp, isMinLen);
							up = up + nMin;
							xt::view(colList, xt::keep(xt::arange<size_t>(xt::sum(!isMinLen)()) + up)) = xt::filter(tmp, !isMinLen);
						}
					}
				}
				if (!noMinFound)
				{
					// Update column reduction values based on new assignment
					colReady = xt::view(colList, xt::keep(xt::arange<size_t>(last)));
					xt::view(colRedux, xt::keep(colReady)) += xt::view(dist, xt::keep(colReady)) - distMin;

					// Set row and column assignments based on shortest path starting at the
					// end of the path and marching backwards until rowFree is reached
					while (true)
					{
						rowAssigned = prevRow.at(endOfPath);
						colAssigned = endOfPath;

						colSoln.at(endOfPath) = rowAssigned;
						endOfPath = (size_t)rowSoln.at(rowAssigned);
						rowSoln.at(rowAssigned) = colAssigned;

						if (rowAssigned == rowFree)
						{
							break;
						}
					}
				}
			}
		}
	}

	static bool isempty(const xt::xarray<double> &arr)
	{
		return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
	}
}