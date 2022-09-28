#include "../../include/assignkbestsd.h"
#include <xtensor/xio.hpp>
#include <xtensor/xview.hpp>
using namespace xt::placeholders;
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
void pds(xt::xarray<double>& d, int k)
{
	std::cout << xt::view(d, xt::all(), xt::all(), k) << std::endl;
}

int main(int argc, char const *argv[])
{
    xt::xarray<double> costMatrix = {{{0, 0, 0},
                                      {0, -6.8, -10.9},
                                      {0, -4.5, -11.1}},
                                     {{0, -10.2, -4.7},
                                      {-6.8, -18, -17},
                                      {-13.2, 0, -9}},
                                     {{0, 0, -5.5},
                                      {-5.2, -14.8, -9.9},
                                      {-10.6, -14.1, -16.7}}};
    costMatrix = xt::transpose(costMatrix, {1, 2, 0});
    std::cout << xt::view(costMatrix, xt::all(), xt::all(), 0) << std::endl;
	std::cout << xt::view(costMatrix, xt::all(), xt::all(), 1) << std::endl;
	std::cout << xt::view(costMatrix, xt::all(), xt::all(), 2) << std::endl;
    std::vector<xt::xarray<size_t>> assignments;
    std::vector<double> cost, gap;
    assignalgo::assignkbestsd(costMatrix,
                              assignments, cost, gap,
                              5, 0.01, 100);

    for (size_t i = 0; i < assignments.size(); ++i)
    {
        std::cout << "assignments[" << i << "] : " << std::endl
                  << assignments.at(i) << std::endl;
        std::cout << "cost[" << i << "]: " << cost.at(i) << std::endl;
        std::cout << "gap[" << i << "]: " << gap.at(i) << std::endl;
    }
    return 0;
}
