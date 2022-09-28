#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xadapt.hpp>
using namespace xt::placeholders;
template <typename T>
bool isempty(xt::xarray<T> arr)
{
    return ((0 == arr.shape().size()) || (0 == xt::prod(xt::adapt(arr.shape()))()));
}
void assignTOMHT(xt::xarray<double> costMatrix,
                 xt::xarray<double> costThreshold,
                 xt::xarray<size_t> &assignments,
                 xt::xarray<size_t> &unassignedTracks,
                 xt::xarray<size_t> &unassignedDetections)
{
    assert(2 == costMatrix.shape().size());
    assert(3 == costThreshold.size());

    // Allocate memory
    size_t numTracks = costMatrix.shape(0);
    size_t numDetections = costMatrix.shape(1);
    unassignedTracks = xt::arange<size_t>(numTracks);
    unassignedDetections = xt::arange<size_t>(numDetections);

    // Calculate all the possible assignments in C3Gate
    xt::xarray<size_t> idx = xt::from_indices(xt::argwhere(costMatrix < costThreshold.at(2)));
    xt::xarray<size_t> rows = xt::col(idx, 0);
    xt::xarray<size_t> columns = xt::col(idx, 1);

    if (!isempty(rows))
    {
        assignments = xt::stack(xt::xtuple(rows, columns), 1);
    }
    else
    {
        assignments = xt::xarray<size_t>::from_shape({0, 2});
    }
    // Find all tracks that have at least one detection in C1Gate
    auto any_functor = xt::make_xreducer_functor([](bool a, bool b) { return a | b; }, xt::const_value<bool>(false));
    xt::xarray<bool> r = !xt::reduce(any_functor, costMatrix < costThreshold.at(0), {1});
    unassignedTracks = xt::filter(unassignedTracks, r);

    // Find all detections that have at least one track in C2Gate
    xt::xarray<bool> t = !xt::reduce(any_functor, costMatrix < costThreshold.at(1), {0});
    unassignedDetections = xt::filter(unassignedDetections, t);
}

#include <xtensor/xio.hpp>
void str_b(xt::xarray<bool> &b)
{
    std::cout << b << std::endl;
}
void str_s(xt::xarray<size_t> &s)
{
    std::cout << s << std::endl;
}
void str_d(xt::xarray<double> &d)
{
    std::cout << d << std::endl;
}
int main(int argc, char const *argv[])
{
    double inf = std::numeric_limits<double>::infinity();
#if 1
    xt::xarray<double> costMatrix = {{4, 9, 200, inf},
                                     {300, 12, 28, inf},
                                     {32, 100, 210, 1000}};
#else
    xt::xarray<double> costMatrix = {{4, 9, 200, inf}};
#endif
    xt::xarray<double> costThreshold = {5, 10, 30};
    xt::xarray<size_t> assignments, unassignedTracks, unassignedDetections;
    assignTOMHT(costMatrix, costThreshold, assignments, unassignedTracks, unassignedDetections);
    std::cout << assignments << std::endl;
    std::cout << unassignedTracks << std::endl;
    std::cout << unassignedDetections << std::endl;
    return 0;
}