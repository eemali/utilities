#include "../include/kbestRemoveDuplicates.h"
#include "../include/kbestRemoveUnassigned.h"
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
namespace assignalgo
{
    std::deque<solution_t> kbestRemoveDuplicates(const std::deque<solution_t> &solutionList,
                                                 const xt::xarray<size_t> &costSize)
    {
        // Find solutions that are duplicates of each other and keep only one copy.
        size_t nSolutions = solutionList.size();
        std::vector<bool> toClearIndex(nSolutions, false);

        size_t costDims = costSize.size();
        // Start for duplicate search from top to bottom. This will ensure the
        // duplicate with higher constraints get deleted.
        xt::xarray<size_t> assignment1, assignment2;
        for (size_t i = 0; i < nSolutions; ++i)
        {
            assignment1 = std::get<1>(solutionList.at(i));
            // The solution can contain unassigned rows and columns
            // as padded assignments. Remove them before comparison.
            if (2 == costDims)
            {
				// CAVEAT: sortrows
				assignment1 = xt::view(assignment1, xt::keep(xt::argsort(xt::col(assignment1, 0))));
                kbestRemoveUnassigned(assignment1, costSize);
            }
            for (size_t j = i + 1; j < nSolutions; ++j)
            {
                assignment2 = std::get<1>(solutionList.at(j));
                // The solution can contain unassigned rows and columns
                // as padded assignments. Remove them before comparison.
                if (2 == costDims)
                {
					// CAVEAT: sortrows
					assignment2 = xt::view(assignment2, xt::keep(xt::argsort(xt::col(assignment2, 0))));
                    kbestRemoveUnassigned(assignment2, costSize);
                }
                if (assignment1 == assignment2)
                {
                    if (std::get<3>(solutionList.at(i)) <= std::get<3>(solutionList.at(j)))
                    {
                        toClearIndex[j] = true;
                    }
                    else
                    {
                        toClearIndex.at(i) = true;
                    }
                }
            }
        }
        std::deque<solution_t> clearedList;
        for (size_t i = 0; i < nSolutions; i++)
        {
            if (!toClearIndex.at(i))
            {
                clearedList.push_back(solutionList.at(i));
            }
        }
        return clearedList;
    }
}