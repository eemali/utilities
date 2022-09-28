#ifndef LAP_AUCTION_H
#define LAP_AUCTION_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    void lapAuction(const xt::xarray<double> &costMatrix,
                    xt::xarray<double> &rowSoln,
                    xt::xarray<double> &colSoln,
                    xt::xarray<double> &colRedux,
                    double maxAuctions = std::numeric_limits<double>::infinity(),
                    bool hasEpsilonScaling = true);
}
#endif // !LAP_AUCTION_H
