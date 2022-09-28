#include "../include/assignsd.h"
#include "../include/assignauction.h"
#include "../include/assignjv.h"
#include "../include/assignmunkres.h"
#include "../include/assignmunkres_fast.h"
#include "../internal/include/assign2DRelaxed.h"
#include "../internal/include/sdRemoveImpossibles.h"
#include <xtensor/xindex_view.hpp>
#include <xtensor/xstrided_view.hpp>
#include <xtensor/xbroadcast.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xnorm.hpp>
#include <xtensor-blas/xlinalg.hpp>
using namespace xt::placeholders;
namespace assignalgo
{
    // Supporting Functions
    //---------------------
    static double computeCostGivenAssignment(const xt::xarray<double> &costMatrix,
                                             const xt::xarray<size_t> &assignments,
                                             const std::vector<xt::xarray<double>> *multiplier = nullptr);
    static xt::xarray<double> constructFeasibleCostMatrix(const xt::xarray<double> &costMatrix,
                                                          const xt::xarray<size_t> &assignments);
    static xt::xarray<double> computeMultiplierGradient(const xt::xarray<size_t> &sortedIndices,
                                                        size_t d1,
                                                        const xt::xarray<size_t> &assignments);
    static void updateLagrangianMultipler(xt::xarray<double> &l,
                                          const xt::xarray<double> &g,
                                          xt::xarray<double> &H,
                                          xt::xarray<double> &p,
                                          double &beta,
                                          double dualCost,
                                          double feasibleCost, bool iterFlag,
                                          bool betaFlag);
    static void updateLagrangianMultiplierHeuristicPrice(xt::xarray<double> &l,
                                                         const xt::xarray<double> &g,
                                                         double dualCost,
                                                         double feasibleCost,
                                                         const xt::xarray<double> &price);
    static std::tuple<xt::xarray<size_t>,
                      double,
                      xt::xarray<double>>
    getSolution(xt::xarray<double> costMatrix,
                assign2dFcn_t algFcn);

    static double computeMultiplierSum(const std::vector<xt::xarray<double>> &multiplier);

    std::tuple<xt::xarray<size_t>, double, double>
    assignsd(xt::xarray<double> costMatrix,
             double desiredGap,
             size_t maxIterations,
             assign2dFcn_t algFcn)
    {
        // Validate Inputs

        // Assess size and type of the problem.
        xt::xarray<size_t> dimSize = xt::adapt(costMatrix.shape());
        size_t numDims = dimSize.size();

        assert(!xt::any(xt::isnan(costMatrix)) && // nonnan
               !isempty(costMatrix) &&            // nonempty
               numDims >= 3 &&                    // expectedAtleast3D
               xt::all(dimSize > 1));             // expectedAtleast2Dims

        assert(&assignauction == algFcn ||
               &assignjv == algFcn ||
               &assignmunkres == algFcn ||
               &assignmunkres_fast == algFcn);

        bool priceUpdate = (&assignauction == algFcn);

        double totalDummyCost = costMatrix.at(0);
        costMatrix.at(0) = 0;

        // We can remove impossibles first so that they don't affect scaling and
        // lagrangian multiplier updates.
        costMatrix = sdRemoveImpossibles(costMatrix);

        // number of constraints = S - 2;
        size_t numConstraints = numDims - 2;

        // Pre-allocation of variables.
        // Lagrange Multiplier (n3x1, n4x1, n5x1 ...)
        // Constrained Cost matrices (n1xn2, n1xn2xn3, n1xn2xn3xn4 ...)

        // The following parameters vary in size for each constraint.
        // Need to store them in a cell.

        std::vector<xt::xarray<double>> lagrangianMultipliers(numConstraints);
        std::vector<xt::xarray<double>> constrainedCost(numConstraints);
        std::vector<xt::xarray<size_t>> costIndices(numConstraints);
        std::vector<xt::xarray<double>> gradients(numConstraints);
        std::vector<xt::xarray<double>> p(numConstraints);
        std::vector<xt::xarray<double>> hessianMats(numConstraints);
        xt::xarray<double> feasibleCost = xt::zeros<double>({numConstraints});
        xt::xarray<double> beta = xt::ones<double>({numConstraints});
        std::vector<xt::xarray<double>> price(numConstraints);
        std::vector<xt::xarray<size_t>> dummyAssignments(numConstraints);

        // Initialize variables
        size_t currentDim;
        for (size_t i = 0; i < numConstraints; ++i)
        {
            currentDim = numDims - i - 1;
            std::vector<size_t> shape(currentDim, 1);
            shape.push_back(dimSize.at(currentDim));
            lagrangianMultipliers.at(i) = xt::zeros<double>(shape);
            constrainedCost.at(i) = xt::zeros<double>(xt::view(dimSize, xt::range(_, currentDim)));
            costIndices.at(i) = xt::zeros<size_t>(xt::view(dimSize, xt::range(_, currentDim)));
            gradients.at(i) = xt::zeros<double>({dimSize.at(currentDim)});
            hessianMats.at(i) = xt::eye<double>(dimSize.at(currentDim));
            p.at(i) = xt::zeros<double>({dimSize.at(currentDim)});
            price.at(i) = xt::zeros<double>({dimSize.at(currentDim)});
            dummyAssignments.at(i) = xt::zeros<size_t>({dimSize.at(currentDim)});
        }

        // Initiate best dual cost so far
        double Inf = std::numeric_limits<double>::infinity();
        double bestDualCost = -Inf;
        xt::xarray<double> bestFeasibleCost = xt::xarray<double>({numConstraints}, Inf);
        xt::xarray<size_t> bestSolution = xt::zeros<size_t>({size_t(0), numDims});

        // Start while loop
        size_t currentIter = 1;
        double currentGap = Inf;
        double bestGap = Inf;

        xt::xarray<double> matrix, allValues, negValues, twoDCost, tempCostMatrix, feasibleCostMatrix;
        xt::xstrided_slice_vector numOnes;
        xt::xarray<size_t> negIndices, constrainedAssignment, validDummies, enforcedConstraint, indAvailable;
        double multiplierSum, dualCost, thisDual;
        xt::xarray<bool> violatedConstraint;
        bool betaUpdateFlag, iterFlag, updateSolution;
        while (currentGap > desiredGap && currentIter - 1 < maxIterations)
        {
            // Step 1: Relax constrains from cost to reach 2-D cost
            // ----------------------------------------------------
            for (size_t i = 0; i < numConstraints; ++i)
            {
                currentDim = numDims - i - 1;
                if (0 == i)
                {
                    matrix = costMatrix - lagrangianMultipliers.at(i);
                }
                else
                {
                    matrix = constrainedCost.at(i - 1) - lagrangianMultipliers.at(i);
                }
				// CAVEAT:
                costIndices.at(i) = xt::argmin(matrix, currentDim);
                constrainedCost.at(i) = xt::nanmin(matrix, {currentDim});
                // constrainedCost.at(i) = xt::view(matrix, xt::keep(costIndices.at(i)));

                // The first index is not a binary variable. Reference [1] Eq. (11)
                // is incorrect when all indices are dummy. For all dummy
                // assignments (index 1), pick the sum of all the negative values.
                numOnes = xt::xstrided_slice_vector(currentDim, 0);
                allValues = xt::strided_view(matrix, numOnes);
                negIndices = xt::flatten_indices(xt::argwhere(allValues < 0));
                negValues = xt::index_view(allValues, negIndices);
                dummyAssignments.at(i).fill(0);
                xt::view(dummyAssignments.at(i), xt::range(_, negIndices.size())) = negIndices;
                constrainedCost.at(i).at(0) = xt::sum(negValues)();
            }

            // Step 2: Solve 2-D assignment problem
            // ------------------------------------
            multiplierSum = computeMultiplierSum(lagrangianMultipliers);
            twoDCost = constrainedCost.back();
            std::tie(constrainedAssignment, std::ignore, std::ignore) = getSolution(twoDCost, algFcn);
            dualCost = computeCostGivenAssignment(twoDCost, constrainedAssignment) + multiplierSum;
            violatedConstraint = xt::xarray<bool>({numConstraints}, true);
            bestDualCost = std::max(dualCost, bestDualCost);
            betaUpdateFlag = bestDualCost != dualCost;

            // Step 3: Add Constraints back
            // ----------------------------
            for (int i = (int)numConstraints - 1; i >= 0; --i)
            {
                currentDim = numConstraints - i + 1;
                if (0 == i)
                {
                    tempCostMatrix = costMatrix;
                }
                else
                {
                    tempCostMatrix = constrainedCost.at(i - 1);
                }
                // Compute gradient for violations
                gradients.at(i) = computeMultiplierGradient(costIndices.at(i), dimSize.at(currentDim), constrainedAssignment);

                // If the dummy solution was picked, add assignments which led to
                // the dummy cost.
                validDummies = xt::filter(dummyAssignments.at(i), dummyAssignments.at(i) > 0);
                xt::index_view(gradients.at(i), validDummies) -= 1;

                // Compute feasible costs 
                multiplierSum = computeMultiplierSum(std::vector<xt::xarray<double>>(lagrangianMultipliers.cbegin(), lagrangianMultipliers.cbegin() + i));
                feasibleCostMatrix = constructFeasibleCostMatrix(tempCostMatrix, constrainedAssignment);

                // Get feasible solution.
                std::tie(enforcedConstraint, std::ignore, price.at(i)) =
                    getSolution(feasibleCostMatrix, algFcn);

                // Enforce feasible solution on the assignment.
                indAvailable = xt::col(enforcedConstraint, 0);

                constrainedAssignment = xt::view(constrainedAssignment, xt::keep(indAvailable));
                xt::xarray<size_t> col = xt::col(enforcedConstraint, 1);
                constrainedAssignment = xt::hstack(xt::xtuple(constrainedAssignment, col.reshape({-1, 1})));

                // Compute feasible cost
                feasibleCost.at(i) = computeCostGivenAssignment(tempCostMatrix, constrainedAssignment) + multiplierSum;

                bestFeasibleCost.at(i) = std::min(bestFeasibleCost.at(i), feasibleCost.at(i));

                if (xt::any(gradients.at(i)))
                {
                    violatedConstraint.at(i) = false;
                }
            }
            updateSolution = bestFeasibleCost.at(0) == feasibleCost.at(0);

            // Step 4: Update Lagrangian Multipliers
            // -------------------------------------
            for (int i = (int)numConstraints - 1; i >= 0; --i)
            {
                // Update Lagrangian Multipliers
                if (i == numConstraints - 1)
                {
                    thisDual = dualCost;
                }
                else
                {
                    thisDual = feasibleCost.at(i + 1);
                }
                if (priceUpdate)
                {
                    updateLagrangianMultiplierHeuristicPrice(lagrangianMultipliers.at(i),
                                                             gradients.at(i),
                                                             thisDual,
                                                             feasibleCost.at(i),
                                                             price.at(i));
                }
                else
                {
                    currentDim = numConstraints - i + 1;
                    // Hessians are p are set to eye and zero every nr iteration. Inform the
                    // update function about it.
                    // Subtract 1 from dimSize to get nr.
                    iterFlag = currentIter % (dimSize.at(currentDim) - 1) == 0;
                    updateLagrangianMultipler(lagrangianMultipliers.at(i),
                                              gradients.at(i), hessianMats.at(i), p.at(i), beta.at(i),
                                              thisDual, feasibleCost.at(i), iterFlag, betaUpdateFlag);
                }
            }

            // Step 5: Store best solution and update iteration information
            // ------------------------------------------------------------
            // Update iteration sweep
            currentIter = currentIter + 1;

            // Edge cases:
            // 1. bestFeasibleCost = Inf
            // 2. bestDualCost = -Inf;
            // 3. bestDualCost ~= bestFeasibleCost. Then adding realmin to denominator can cause current gap to become negative.
            // 4. bestDualCost == bestFeasibleCost = +/-Inf. currentGap will be NaN, which will be reduced to 0 by the max function
            double sign = bestFeasibleCost.at(0) > 0 ? 1 : bestFeasibleCost.at(0) < 0 ? -1
                                                                                      : 0;
            currentGap = sign - bestDualCost / std::abs(bestFeasibleCost.at(0) + std::numeric_limits<double>::min());
            currentGap = std::max(0., currentGap);
            bestGap = std::min(currentGap, bestGap);

            if (updateSolution || currentIter == 2)
            {
                bestSolution = constrainedAssignment;
                bestGap = currentGap;
            }

            if (xt::all(violatedConstraint))
            {
                bestGap = 0.;
                break;
            }
        } // End of while loop

        // Step 6: Return best solution found
        // ----------------------------------
        // First solution is a dummy. Accept it if the cost is negative.
        xt::xarray<size_t> assignments;
        if (totalDummyCost >= 0)
        {
            assignments = xt::view(bestSolution, xt::range(1, _));
        }
        else
        {
            assignments = bestSolution;
        }
        costMatrix.at(0) = totalDummyCost;
        double costOfAssignment = computeCostGivenAssignment(costMatrix, assignments);
        double solutionGap = bestGap;
        return std::make_tuple(std::move(assignments), costOfAssignment, solutionGap);
    }
    // Supporting Functions
    //---------------------
    // Computing Cost given assignments and multipliers
    static double computeCostGivenAssignment(const xt::xarray<double> &costMatrix,
                                             const xt::xarray<size_t> &assignments,
                                             const std::vector<xt::xarray<double>> *multiplier)

    {
        // This function takes a cost matrix of any dimension and assignments
        // satisfying the dimensions of the costMatrix and returns the cost of total
        // assignment.
        double cost = 0;
        xt::xarray<size_t> idx;
        for (size_t i = 0; i < assignments.shape(0); ++i)
        {
            idx = xt::row(assignments, i);
            cost += xt::strided_view(costMatrix, xt::xstrided_slice_vector(idx.cbegin(), idx.cend()))();
        }
        double lSum = (nullptr == multiplier) ? 0 : computeMultiplierSum(*multiplier);
        cost += lSum;
        return cost;
    }

    // Construct feasible cost matrix
    static xt::xarray<double> constructFeasibleCostMatrix(const xt::xarray<double> &costMatrix,
                                                          const xt::xarray<size_t> &assignments)
    {
        // This function generates the cost matrix to construct a feasible solution,
        // given a constrained solution.
        size_t d1 = assignments.shape(0);
        size_t currentDim = costMatrix.dimension() - 1;
        size_t d2 = costMatrix.shape(currentDim);

        xt::xarray<double> feasibleCostMatrix = xt::zeros<double>({d1, d2});
        for (size_t i = 0; i < d1; ++i)
        {
            xt::xstrided_slice_vector sv;
            for (size_t j = 0; j < currentDim; ++j)
            {
                sv.push_back(assignments.at(i, j));
            }
            xt::row(feasibleCostMatrix, i) = xt::strided_view(costMatrix, sv);
        }
        return feasibleCostMatrix;
    }

    // Compute gradient
    static xt::xarray<double> computeMultiplierGradient(const xt::xarray<size_t> &sortedIndices,
                                                        size_t d1,
                                                        const xt::xarray<size_t> &assignments)
    {
        // This function computes the gradient for each dimension. The gradient
        // calculation is based on the following approach:
        // 1. The index 1 has 0 gradient i.e. it is not punished for 0 or multiple
        // assignments.
        // 2. Other indices get gradient based on how many assignments contain them.
        // For example, if index 5 is assigned 3 times, it's gradient is +2 i.e. it
        // needs to be punished by adding cost. If it's assigned 1 times,
        // it's gradient is 0. If it's not assigned it's gradient is -1 i.e. it
        // needs to be punished by subtracting cost.
        xt::xarray<size_t> indicesUsed = xt::zeros<size_t>({assignments.shape(0) - 1});
        size_t currentDim = sortedIndices.dimension();
        xt::xarray<double> g = xt::zeros<size_t>({d1});
        xt::xarray<size_t> idx;
        for (size_t i = 1; i < assignments.shape(0); ++i)
        {
            idx = xt::row(assignments, i);
            indicesUsed.at(i - 1) = xt::strided_view(sortedIndices, xt::xstrided_slice_vector(idx.cbegin(), idx.cend()))();
        }
        for (size_t i = 1; i < d1; ++i)
        {
            g.at(i) = 1. - xt::sum(xt::equal(indicesUsed, i))();
        }
        return g;
    }

    // Lagrangian Multiplier Update via Accelerated Sub-gradient Descent
    static void updateLagrangianMultipler(xt::xarray<double> &l,
                                          const xt::xarray<double> &g,
                                          xt::xarray<double> &H,
                                          xt::xarray<double> &p,
                                          double &beta,
                                          double dualCost,
                                          double feasibleCost, bool iterFlag,
                                          bool betaFlag)
    {
        // The Lagrangian multipliers are updated using a accelerated sub-gradient
        // method from [1].

        // Method - specific constants
        // Recommended values.
        double alpha = 2;
        double ar = 0.02;
        double br = 1.15;

        // Eq. (18)
        if (betaFlag)
        {
            beta = beta + 1;
        }
        else
        {
            beta = std::max(beta - 1, 1.);
        }
        double adaptCost = (1 + ar / std::pow(beta, br)) * dualCost;
        double thisGap = feasibleCost - adaptCost;

        // When gap is infinite, take a small step. Detailed explanation in
        // heuristic price update method.
        if (!std::isfinite(thisGap))
        {
            thisGap = 0.1 * std::abs(adaptCost);
        }

        // Eq (17)
        if (!iterFlag)
        {
            p = xt::linalg::dot(H, g);
            double den = xt::linalg::vdot(p, g);
            if ((bool)den)
            {
                H += (1 - std::pow(alpha, -2)) * xt::linalg::outer(p, p) / den;
            }
        }
        else
        {
            H = xt::eye<double>(H.shape(0));
            p.fill(0);
        }
        xt::xarray<double> adaptFactor = (((alpha + 1) / alpha) * thisGap * p) / xt::norm_sq(g)();
        if (xt::all(xt::isfinite(adaptFactor)))
        {
            xt::flatten(l) += adaptFactor;
        }
    }

    static void updateLagrangianMultiplierHeuristicPrice(xt::xarray<double> &l,
                                                         const xt::xarray<double> &g,
                                                         double dualCost,
                                                         double feasibleCost,
                                                         const xt::xarray<double> &price)
    {
        // This function updates the Lagrangian multipliers using a Heuristic price
        // update method described in [1].
        double thisGap = feasibleCost - dualCost;
        // When feasibleCost is Inf, dualCost is some scalar, we need to decide how
        // large a step to take in those cases. A huge step can greatly reduce the
        // dualCost to a point beyond recovery. A small step is okay, but will take
        // longer to converge, which is better than no convergence. Here, we assume
        // that we can increase the dualCost by 1%.
        if (!std::isfinite(thisGap))
        {
            thisGap = 0.1 * std::abs(dualCost);
        }
        double gapFactor = thisGap / xt::norm_sq(g)();
        int dimSize = (int)price.size() - 1;
        double sumPrice = xt::sum(price)();
        xt::xarray<double> priceFactor = dimSize * price * g;
        xt::xarray<double> adaptFactor = (gapFactor * priceFactor) / sumPrice;
        if (xt::all(xt::isfinite(adaptFactor)))
        {
            xt::flatten(l) += adaptFactor;
        }
    }
    // Get solution to assignment problem
    static std::tuple<xt::xarray<size_t>,
                      double,
                      xt::xarray<double>>
    getSolution(xt::xarray<double> costMatrix,
                assign2dFcn_t algFcn)
    {
        return assign2DRelaxed(costMatrix, algFcn);
    }

    static double computeMultiplierSum(const std::vector<xt::xarray<double>> &multiplier)
    {
        // Compute the sum of lagrangian multipliers.
        double lSum = 0;
        for (const xt::xarray<double> &l : multiplier)
        {
            lSum += xt::sum(l)();
        }
        return lSum;
    }
}
