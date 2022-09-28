#ifndef ASSIGN_AUCTION_WITH_PRICE_H
#define ASSIGN_AUCTION_WITH_PRICE_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    void assignAuctionWithPrice(xt::xarray<double> costMatrix,
                                xt::xarray<size_t> &assignments,
                                xt::xarray<size_t> &unassignedRows,
                                xt::xarray<size_t> &unassignedColumns,
                                xt::xarray<double> &price);
}
#endif // !ASSIGN_AUCTION_WITH_PRICE_H
