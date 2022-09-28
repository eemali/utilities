#ifndef K_BEST_SD_PARTITION_SOLUTION
#define K_BEST_SD_PARTITION_SOLUTION
#include <xtensor/xarray.hpp>
#include <tuple>
#include <deque>
namespace assignalgo
{
    typedef void (*algFcn_t)(xt::xarray<double>,
                             double,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &,
                             xt::xarray<size_t> &);

    typedef std::tuple<xt::xarray<double>, xt::xarray<size_t>, std::vector<double>, size_t> solution_t;

    std::deque<solution_t> kbestSDPartitionSolution(const solution_t &solution,
                                                    double desiredGap,
                                                    size_t maxIterations,
                                                    algFcn_t algFcn);
}
#endif // !K_BEST_SD_PARTITION_SOLUTION