#include "../include/helperSphericalIntersection.h"
#include <xtensor/xsort.hpp>
#include <xtensor/xnorm.hpp>
#include <xtensor/xview.hpp>
#include <xtensor-blas/xlinalg.hpp>
xt::xarray<double> helperSphericalIntersection(const xt::xarray<double> &r,
                                               xt::xarray<double> receiverLocations,
                                               const xt::xarray<double> &emitterLocation)
{
    receiverLocations = receiverLocations - xt::reshape_view(emitterLocation, {emitterLocation.size(), size_t(1)});
    xt::xarray<double> S = xt::transpose(receiverLocations);
    xt::xarray<double> z = 0.5 * (xt::sum(S * S, 1) - r * r);

    // Less than 3 detections cannot be fused.
    // Put a target at a position with low likelihood.
    int S_rank = xt::linalg::matrix_rank(S);
    if (S_rank < 2)
    {
        return xt::xarray<double>({3, 1}, 1e10);
    }
    xt::xarray<double> S_pinv = xt::linalg::pinv(S);
    xt::xarray<double> a = xt::linalg::dot(S_pinv, z); // std::get<0>(xt::linalg::lstsq(S, z)); // Eq 17
    xt::xarray<double> b = xt::linalg::dot(S_pinv, r); // std::get<0>(xt::linalg::lstsq(S, r)); // Eq 18

    double aTb = xt::linalg::vdot(a, b);
    double bTb = xt::linalg::vdot(b, b);
    double aTa = xt::linalg::vdot(a, a);

    xt::xarray<double> tgtRanges = xt::zeros<double>({2}); // Two solutions
    double delta = std::pow(aTb, 2) - (bTb - 1) * aTa;
    delta = delta > 0 ? std::sqrt(delta) : 0;
    tgtRanges.at(0) = (-aTb + delta) / (bTb - 1); // Eq 21
    tgtRanges.at(1) = (-aTb - delta) / (bTb - 1); // Eq 21

    xt::xarray<double> estPos = xt::zeros<double>({3, 2});
    xt::col(estPos, 0) = a + b * tgtRanges.at(0);
    xt::col(estPos, 1) = a + b * tgtRanges.at(1);

    // if one is positive z and one is negative z the decision is easy.
    size_t chosenID;
    if (estPos.at(2, 0) * estPos(2, 1) < 0)
    {
        chosenID = xt::argmin(xt::row(estPos, 2))();
    }
    else
    {
        // When more than minimum number of receivers are available, error
        // norm can be used to choose the solution.
        xt::xarray<double> col1 = xt::view(estPos, xt::all(), xt::range(0, 1));
        xt::xarray<double> col2 = xt::view(estPos, xt::all(), xt::range(1, 2));
        xt::xarray<double> tgtToRec1 = xt::view(estPos, xt::all(), xt::range(0, 1)) - receiverLocations;
        xt::xarray<double> tgtToRec2 = xt::view(estPos, xt::all(), xt::range(1, 2)) - receiverLocations;
        xt::xarray<double> tgtToEmit1 = xt::col(estPos, 0);
        xt::xarray<double> tgtToEmit2 = xt::col(estPos, 1);
        xt::xarray<double> expRanges1 = xt::norm_l2(tgtToRec1, {0}) + xt::norm_l2(tgtToEmit1);
        xt::xarray<double> expRanges2 = xt::norm_l2(tgtToRec2, {0}) + xt::norm_l2(tgtToEmit2);

        xt::xarray<double> errorNorm = xt::zeros<double>({2});
        errorNorm.at(0) = xt::norm_l2(r - expRanges1)();
        errorNorm.at(1) = xt::norm_l2(r - expRanges2)();

        chosenID = xt::argmin(errorNorm)();
    }
    // Add back emitter location
    return xt::col(estPos, chosenID) + emitterLocation;
}