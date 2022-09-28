#ifndef ASSIGN_AUCTION_WITH_PRICE_H
#define ASSIGN_AUCTION_WITH_PRICE_H
#include <xtensor/xarray.hpp>
namespace assignalgo
{
    /**
     * @brief Linear Assignment Problem using Auction Algorithm
     * using price returned.
     * 
     * It solves the 2-D
     * assignment problem using Forward reverse auction algorithm. price are the
     * values of dual variables when the solution was found.
     * 
     * The price from auction algorithm can be used to update the Lagrangian
     * Multipliers for the S-D assignment problem more efficiently.
     * 
     * Note that this function does not solve the Generalized Assignment problem 
     * where costMatrix can be rectangular. Therefore, costMatrix input must be 
     * padded with rows and columns for unassignment before passing to this 
     * function. 
     */
    std::tuple<xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<size_t>,
               xt::xarray<double>>
    assignAuctionWithPrice(xt::xarray<double> costMatrix);
}
#endif // !ASSIGN_AUCTION_WITH_PRICE_H
