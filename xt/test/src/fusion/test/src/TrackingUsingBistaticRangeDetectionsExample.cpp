#include <xtensor/xarray.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xnorm.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor-blas/xlinalg.hpp>
#include <tuple>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../include/staticDetectionFuser.h"
#include "../include/helperBistaticRangeFusion.h"
#include "../include/helperBistaticMeas.h"
using namespace xt::placeholders;
std::vector<double> line2dvec(const std::string &line)
{
    std::stringstream sline(line);
    std::string s;
    std::vector<double> v;
    while (std::getline(sline, s, ','))
    {
        v.push_back(std::stod(s));
    }
    return v;
}
objectDetection make_detection(const std::vector<double> &v)
{
    objectDetection det;
    det.Time = v[0];
    det.Measurement = xt::xarray<double>({v[1]});
    det.MeasurementNoise = xt::xarray<double>({v[2]});
    det.SensorIdx = v[3];
    det.ObjectClassID = v[4];
    det.MeasurementParameters = {{Frame_t::SPHERICAL,
                                  xt::zeros<double>({3}),
                                  xt::zeros<double>({3}),
                                  xt::eye<double>(3),
                                  true,
                                  false,
                                  false,
                                  true,
                                  false,
                                  xt::zeros<double>({3})},
                                 {Frame_t::RECTANGULAR,
                                  xt::xarray<double>({v[5], v[6], v[7]}),
                                  xt::zeros<double>({3}),
                                  xt::eye<double>(3),
                                  true,
                                  true,
                                  true,
                                  true,
                                  false,
                                  xt::xarray<double>()}};
    det.ObjectAttributes = {{6, 1, 1, v[8]}};
    return det;
}
bool fuse_bistatic_dets_wofa(void)
{
    std::string filename = "bistatic_dets_wofa.csv";
    std::ifstream fp(filename);
    if (!fp.is_open())
    {
        std::cerr << "\x1B[31m"
                  << "Error: couldn't able to open " + filename + " for reading"
                  << "\033[0m" << std::endl;
        return false;
    }
    else
    {
        std::string hline;
        std::getline(fp, hline);
    }

    std::string line;
    std::vector<objectDetection> detections;
    std::cout << "========================================" << std::endl;
    std::cout << "# | pos_error_linf | cov_error_linf" << std::endl;
    int j = 0;
    while (std::getline(fp, line))
    {
        std::vector<double> vals = line2dvec(line);
        if (9 == vals.size())
        {
            detections.push_back(make_detection(vals));
        }
        else if (12 == vals.size())
        {
            xt::xarray<double> pos({vals[0], vals[1], vals[2]});
            xt::xarray<double> cov({{vals[3], vals[4], vals[5]},
                                    {vals[6], vals[7], vals[8]},
                                    {vals[9], vals[10], vals[11]}});
            xt::xarray<double> position, covariance;
            std::tie(position, covariance) = helperBistaticRangeFusion(detections, true);
            j = j + 1;
            detections.clear();
            //std::cout << "pos:" << std::endl
            //          << pos << std::endl;
            //std::cout << "position:" << std::endl
            //          << position << std::endl;
            //std::cout << "pos_error_linf: " << xt::norm_linf(pos - position) << std::endl;
            //std::cout << "cov:" << std::endl
            //          << cov << std::endl;
            //std::cout << "covariance:" << std::endl
            //          << covariance << std::endl;
            //std::cout << "cov_error_linf: " << xt::norm_linf(cov - covariance) << std::endl;
            std::cout << j << " | " << std::fixed << std::setprecision(16)
                      << xt::norm_linf(pos - position) << " | "
                      << xt::norm_linf(cov - covariance) << std::endl;
        }
        else
        {
            XTENSOR_THROW(std::runtime_error, "Invalid vals");
        }
    }
    fp.close();
    return true;
}
bool fuse_bistatic_dets_wfa(void)
{
    std::string filename = "bistatic_dets_wfa.csv";
    std::ifstream fp(filename);
    if (!fp.is_open())
    {
        std::cerr << "\x1B[31m"
                  << "Error: couldn't able to open " + filename + " for reading"
                  << "\033[0m" << std::endl;
        return false;
    }
    else
    {
        std::string hline;
        std::getline(fp, hline);
    }

    std::string line;
	staticDetectionFuser fuser(&helperBistaticRangeFusion, &helperBistaticMeas, 4, 1, 0.99, 1e-6, 100, 1e-6);
    std::vector<objectDetection> detections;
    std::vector<objectDetection> superDets;
    info_t info;
    int i = -1;
    int j = 0;
    while (std::getline(fp, line))
    {
        std::vector<double> vals = line2dvec(line);
        if (9 == vals.size())
        {
            detections.push_back(make_detection(vals));
            i = -1;
        }
        else if (12 == vals.size())
        {
            xt::xarray<double> pos({vals[0], vals[1], vals[2]});
            xt::xarray<double> cov({{vals[3], vals[4], vals[5]},
                                    {vals[6], vals[7], vals[8]},
                                    {vals[9], vals[10], vals[11]}});
            if (-1 == i)
            {
                std::tie(superDets, info) = fuser.fuse(detections);
                detections.clear();
                j = j + 1;
                std::cout << "================[" << j << "]================" << std::endl;
                std::cout << "pos_error_linf | cov_error_linf" << std::endl;
            }
            i = i + 1;
            xt::xarray<double> position = superDets.at(i).Measurement;
            xt::xarray<double> covariance = superDets.at(i).MeasurementNoise;
            //std::cout << "pos:" << std::endl
            //	<< pos << std::endl;
            //std::cout << "position:" << std::endl
            //	<< position << std::endl;
            //std::cout << "pos_error_linf: " << xt::norm_linf(pos - position) << std::endl;
            //std::cout << "cov:" << std::endl
            //	<< cov << std::endl;
            //std::cout << "covariance:" << std::endl
            //	<< covariance << std::endl;
            //std::cout << "cov_error_linf: " << xt::norm_linf(cov - covariance) << std::endl;
            std::cout << std::fixed << std::setprecision(16)
                      << xt::norm_linf(pos - position) << " | " << xt::norm_linf(cov - covariance) << std::endl;
        }
        else
        {
            XTENSOR_THROW(std::runtime_error, "Invalid vals");
        }
    }
    fp.close();
    return true;
}
int main(int argc, char const *argv[])
{
    bool rv_wofa = fuse_bistatic_dets_wofa();
    bool rv_wfa = fuse_bistatic_dets_wfa();
    return 0;
}

void pb(xt::xarray<bool> &b)
{
    std::cout << b << std::endl;
}
void ps(xt::xarray<size_t> &s)
{
    std::cout << s << std::endl;
}
void pi(xt::xarray<int> &i)
{
    std::cout << i << std::endl;
}
void pd(xt::xarray<double> &d)
{
    std::cout << d << std::endl;
}
void pc(xt::xarray<double> &c, int i, int j)
{
    std::cout << xt::view(c, xt::all(), xt::all(), i, j) << std::endl;
}