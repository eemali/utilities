#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xrandom.hpp"
#include "xtensor/xbuilder.hpp"
#include "xtensor/xbroadcast.hpp"
#include <iostream>
#include <algorithm>
int main(int argc, char **argv)
{
	//[1]
	xt::xarray<double> arr1{{1.0, 2.0, 3.0},
							{2.0, 5.0, 7.0},
							{2.0, 5.0, 7.0}};

	xt::xarray<double> arr2{5.0, 6.0, 7.0};

	std::cout << xt::view(arr1, 1) + arr2 << std::endl;

	//[2]
	xt::xarray<int> arr{1, 2, 3, 4, 5, 6, 7, 8, 9};

	std::cout << arr.reshape({3, 3}) << std::endl;

	//[3]
	xt::xarray<double> arr3{1.0, 2.0, 3.0};

	xt::xarray<unsigned int> arr4{4, 5, 6, 7};

	arr4.reshape({4, 1});

	std::cout << xt::pow(arr3, arr4) << std::endl;

	//[4]
	xt::xarray<double> arr5 = xt::random::randn<double>({4, 3});
	std::cout << arr5 << std::endl;
	//std::cout << (xt::random::randn<double>({ 5, 3, 8, 10 }) > 0.2) << std::endl;

	//[5]
	xt::xarray<double> ar = xt::linspace<double>(0.0, 10.0, 12);
	std::cout << ar << std::endl;
	ar.reshape({4, 3});
	std::cout << ar << std::endl;

	//[6]
	xt::xarray<double> fones = xt::ones<float>({2, 2});
	std::cout << fones << std::endl;

	//[7]
	std::cout << xt::broadcast(xt::linspace<double>(0.0, 10.0, 4),
							   std::vector<std::size_t>({3, 4}))
			  << std::endl;

	//[8]
	xt::xarray<double> frand = xt::random::randn<double>({2, 2});

	// begin() and end() provide and iterator pair iterating over the xexpression in a row-major fashion
	std::cout << std::accumulate(frand.begin(), frand.end(), 0.0) << std::endl;

	std::cout << frand << std::endl;

	//[9]
	// begin(shape) and end(shape) provide and iterator pair iterating
	// over the xexpression broadcasted to the prescrived shape in a row-major fashion
	std::vector<std::size_t> shape = {3, 2, 2};
	std::cout << std::accumulate(frand.begin(shape), frand.end(shape), 0.0) << std::endl;

	return 0;
}