#include "../../include/assignTOMHT.h"
#include <xtensor/xio.hpp>
int main(int argc, char const *argv[])
{
    double Inf = std::numeric_limits<double>::infinity();
    xt::xarray<double> costMatrix = {{4, 9, 200, Inf},
                                     {300, 12, 28, Inf},
                                     {32, 100, 210, 1000}};
    xt::xarray<double> costThreshold = {5, 10, 30};

    xt::xarray<size_t> assignments, unassignedTracks, unassignedDetections;

    std::tie(assignments, unassignedTracks, unassignedDetections) =
        assignalgo::assignTOMHT(costMatrix, costThreshold);

    std::cout << costMatrix << std::endl;           // {{4., 9., 200., inf}, {300., 12., 28., inf}, {32., 100., 210., 1000.}}
    std::cout << costThreshold << std::endl;        // {5., 10., 30.}
    std::cout << assignments << std::endl;          // {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
    std::cout << unassignedTracks << std::endl;     // {1, 2}
    std::cout << unassignedDetections << std::endl; // {2, 3}
    return 0;
}