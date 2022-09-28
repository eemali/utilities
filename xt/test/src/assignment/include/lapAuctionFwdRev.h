#ifndef LAP_AUCTION_FORWARD_REVERSE_H
#define LAP_AUCTION_FORWARD_REVERSE_H
#include <xtensor/xadapt.hpp>
namespace assignalgo
{
    void lapAuctionFwdRev(xt::xarray<double> &costMatrix,
                          xt::xarray<double> &rowSoln,
                          xt::xarray<double> &colSoln,
                          xt::xarray<double> &colRedux,
                          double maxAuctions = std::numeric_limits<double>::infinity(),
                          bool hasEpsilonScaling = true);
}
#endif // !LAP_AUCTION_FORWARD_REVERSE_H
