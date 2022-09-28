#include <xtensor/xarray.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xnorm.hpp>
#include <xtensor-blas/xlinalg.hpp>
xt::xarray<double> helperSphericalIntersection(xt::xarray<double> &r,
                                               xt::xarray<double> &receiverLocations,
                                               xt::xarray<double> &emitterLocation)
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
    xt::xarray<double> a = std::get<0>(xt::linalg::lstsq(S, z)); // Eq 17
    xt::xarray<double> b = std::get<0>(xt::linalg::lstsq(S, r)); // Eq 18

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
        xt::xarray<double> tgtToRec1 = xt::col(estPos, 0) - receiverLocations;
        xt::xarray<double> tgtToRec2 = xt::col(estPos, 1) - receiverLocations;
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
#include <xtensor/xio.hpp>
int main(int argc, char const *argv[])
{
    xt::xarray<double> r = xt::xarray<double>{4909.30578291239, 4503.92862571888, 3291.78085631687, 2779.26334964790};
    xt::xarray<double> receiverLocations = xt::xarray<double>{{1935.64204728606, 1048.14782395236, -834.459765904092, -1931.90205587064},
                                                              {194.060727917151, 1710.97484035145, 1603.76749001003, 182.921621919518},
                                                              {-380.975836177692, -832.750769456557, -700.965939404046, -63.9031398916126}};
    xt::xarray<double> emitterLocation = xt::xarray<double>{0, 0, 0};

    xt::xarray<double> triangulatedPos = helperSphericalIntersection(r, receiverLocations, emitterLocation);
    std::cout << triangulatedPos << std::endl;
    return 0;
}
void pb(xt::xarray<bool> &b)
{
    std::cout << b << std::endl;
}
void ps(xt::xarray<size_t> &s)
{
    std::cout << s << std::endl;
}
void pd(xt::xarray<double> &d)
{
    std::cout << d << std::endl;
}