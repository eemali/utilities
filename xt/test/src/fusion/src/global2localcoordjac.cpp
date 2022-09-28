#include "../include/global2localcoordjac.h"
#include <xtensor/xview.hpp>
#include <xtensor-blas/xlinalg.hpp>
using namespace xt::placeholders;
xt::xarray<double> global2localcoordjac(const xt::xarray<double> &tgtpos,
                                        const xt::xarray<double> &sensorpos,
                                        const xt::xarray<double> &laxes)
{
    xt::xarray<double> relpos = tgtpos - sensorpos;
    xt::xarray<double> relposlocal = xt::linalg::dot(xt::transpose(laxes), relpos);
    double xrel = relposlocal.at(0);
    double yrel = relposlocal.at(1);
    double zrel = relposlocal.at(2);
    double xysq = std::pow(xrel, 2) + std::pow(yrel, 2);
    double xyzsq = std::pow(xrel, 2) + std::pow(yrel, 2) + std::pow(zrel, 2);
    xt::xarray<double> A = xt::zeros<double>({3, 3});

    if (0 == xyzsq) // The target and the sensor are colocated
    {
        A = xt::linalg::dot(laxes, xt::vstack(xt::xtuple(xt::zeros<double>({2, 3}),
                                                         xt::ones<double>({1, 3}))));
    }
    else if (0 == xysq) // The target has the same (x,y) but not the same z
    {
        double x = -1.0 / zrel * 180.0 / xt::numeric_constants<double>::PI;
        A = xt::linalg::dot(laxes, xt::vstack(xt::xtuple(xt::zeros<double>({1, 3}),
                                                         xt::xarray<double>({{x, x, 0}}),
                                                         xt::xarray<double>({{0, 0, 1}}))));
    }
    else // The normal case
    {
        // Since the local coordinates depend on the global coordinates, need to
        // use the chain rule to account for the partial derivatives of local
        // coordinates with respect to global coordinates.
        xt::row(A, 0) = xt::linalg::dot(laxes, xt::xarray<double>({-yrel, xrel, 0}) / xysq);
        xt::row(A, 1) = xt::linalg::dot(laxes, xt::xarray<double>({-xrel * zrel, -yrel * zrel, xysq}) / std::sqrt(xysq) / xyzsq);
        xt::row(A, 2) = xt::linalg::dot(laxes, xt::xarray<double>({xrel, yrel, zrel}) / std::sqrt(xyzsq));
        xt::view(A, xt::range(_, 2)) *= 180.0 / xt::numeric_constants<double>::PI;
    }
    return A;
}