#include "../../include/assignkbest.h"
#include <xtensor/xio.hpp>
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
int main(int argc, char const *argv[])
{
    double inf = std::numeric_limits<double>::infinity();
    xt::xarray<double> costMatrix = {{10, 5, 8, 9},
                                     {7, inf, 20, inf},
                                     {inf, 21, inf, inf},
                                     {inf, 15, 17, inf},
                                     {inf, inf, 16, 22}};
    double costOfNonAssignment = 100;

    std::vector<xt::xarray<size_t>> assignments, unassignedRows, unassignedColumns;
    std::vector<double> cost;
    size_t numSolutions = 5;
    assignalgo::assignkbest(costMatrix, costOfNonAssignment,
                            assignments, unassignedRows, unassignedColumns, cost,
                            numSolutions);
    for (size_t i = 0; i < assignments.size(); ++i)
    {
        std::cout << "assignments[" << i << "] : " << std::endl
                  << assignments.at(i) << std::endl;
        std::cout << "unassignedRows[" << i << "]: " << unassignedRows.at(i) << std::endl;
        std::cout << "unassignedColumns[" << i << "] : " << unassignedColumns.at(i) << std::endl;
        std::cout << "cost[" << i << "]: " << cost.at(i) << std::endl;
    }
    return 0;
}
