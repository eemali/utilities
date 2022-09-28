#include "./assignalgo/include/assignsd.h"
#include "./assignalgo/include/assignauction.h"
#include "./assignalgo/include/assignjv.h"
#include "./assignalgo/include/assignmunkres.h"
#include "./assignalgo/include/assignmunkres_fast.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp>
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
int main(int argc, char const *argv[])
{
    std::string line = "0.0000000000000000,0.0000000000000000,0.0000000000000000,0.0000000000000000,0.0000000000000000,0.0000000000000000,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.4149835370626747,-35.4108072791911184,-35.4110437050024842,Inf,Inf,Inf,Inf,-35.4079829296106112,-35.4082193554219771,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.2587306676565433,-35.2589670934679091,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-34.4554821777150266,Inf,Inf,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,-35.3345484396956522,-35.3303721818240959,-35.3306086076354617,Inf,Inf,Inf,-35.3499596531331761,-35.3457833952616198,-35.3460198210729857,Inf,Inf,Inf,-35.3471353035526619,-35.3429590456811127,-35.3431954714924714,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.2350662595408011,-31.9511652717599617,Inf,Inf,Inf,-35.4117158596954198,-35.4075396018238635,-35.4077760276352294,Inf,Inf,Inf,-35.4088915101149126,-35.4047152522433564,-35.4049516780547222,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,-35.3881948030220173,-35.3840185451504539,-35.3842549709618197,Inf,Inf,Inf,-35.4036060164595341,-35.3994297585879849,-35.3996661843993436,Inf,Inf,Inf,-35.4007816668790269,-35.3966054090074707,-35.3968418348188365,Inf,Inf,Inf,Inf,Inf,Inf,-34.5807700552089372,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.4013332191922530,-35.3971569613206967,-35.3973933871320625,Inf,Inf,-35.3870126672505663,Inf,Inf,Inf,Inf,Inf,-35.4024238806880902,Inf,Inf,Inf,Inf,Inf,-35.3995995311075831,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-34.7877132908261331,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-34.9198770516095109,Inf,Inf,Inf,-52.3876572773693141,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.3980655418249981,-35.3938892839534418,-35.3941257097648077,Inf,Inf,-35.3837449898833114,Inf,Inf,Inf,Inf,Inf,-35.3991562033208353,Inf,Inf,Inf,Inf,Inf,-35.3963318537403211,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,-1.5532360436073418,-35.3948961809773266,-35.3951326067886924,Inf,Inf,Inf,-35.4144836522864068,-35.4103073944148505,-35.4105438202262164,Inf,Inf,Inf,-35.4116593027058997,-35.4074830448343434,-35.4077194706457092,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.4122108550191257,-35.4080345971475694,-35.4082710229589281,Inf,Inf,-35.3978903030774390,Inf,Inf,Inf,Inf,Inf,-35.4133015165149629,Inf,-52.9966571647866829,-53.0945956780322774,Inf,Inf,-35.4104771669344487,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.4089431776518708,-35.4047669197803145,-35.4050033455916804,Inf,Inf,-35.3946226257101841,Inf,Inf,Inf,Inf,Inf,-35.4100338391477010,Inf,Inf,Inf,Inf,Inf,-35.4072094895671938,Inf,-24.1618560472017805,-25.0178433745472759,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,-35.3986509145965442,-35.3944746567249879,-35.3947110825363467,Inf,Inf,Inf,-35.4140621280340611,-35.4098858701625119,-35.4101222959738706,Inf,Inf,Inf,-35.4112377784535539,-35.4070615205819976,-35.4072979463933635,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.4117893307667799,-35.4076130728952236,-35.4078494987065895,Inf,Inf,-35.3974687788250932,Inf,Inf,Inf,Inf,Inf,-35.4128799922626172,Inf,-44.2747980077649288,-43.7322542876576037,Inf,Inf,-35.4100556426821100,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.4085216533995251,-35.4043453955279688,-35.4045818213393346,Inf,Inf,-35.3942011014578384,Inf,Inf,Inf,Inf,Inf,-35.4096123148953623,-31.2769377997864026,Inf,Inf,Inf,Inf,-35.4067879653148481,Inf,-49.0608951441095655,-49.3875295117624802,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,0.0000000000000000,Inf,Inf,Inf,Inf,Inf,Inf,-35.3126426955086714,-35.3084664376371080,-35.3087028634484739,Inf,Inf,Inf,-35.3280539089461882,-35.3238776510746320,-35.3241140768859978,Inf,Inf,Inf,-35.3252295593656811,-35.3210533014941248,-35.3212897273054907,Inf,Inf,Inf,Inf,Inf,Inf,-34.5052179476955914,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.1849394557102997,-35.1851758815216584,-34.7121611833127872,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-34.8443249440961580,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,-35.2607572277494086,-35.2565809698778452,-35.2568173956892110,-34.7838026974803327,Inf,-35.2464366758077148,-35.5432373971626419,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf,Inf";
    std::vector<double> v = line2dvec(line);
    std::vector<std::size_t> shape = {5, 5, 6, 6};
    xt::xarray<double> costMatrix = xt::adapt(v, shape);
	
	xt::xarray<size_t> assignments_auction, assignments_jv, assignments_munkres, assignments_munkres_fast;
	std::tie(assignments_auction, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignauction);
	std::tie(assignments_jv, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignjv);
	std::tie(assignments_munkres, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignmunkres);
	std::tie(assignments_munkres_fast, std::ignore, std::ignore) = assignalgo::assignsd(costMatrix, 0.05, 200, &assignalgo::assignmunkres_fast);

    return 0;
}

template <typename T>
void pa(xt::xarray<T> &a, bool b)
{
    switch (a.dimension())
    {
    case 3:
        for (size_t d2 = 0; d2 < a.shape(2); d2++)
        {
            std::cout << "val(:,:," << d2 + b << ") = " << std::endl
                      << xt::print_options::precision(16) << xt::view(a, xt::all(), xt::all(), d2) << std::endl
                      << std::endl;
        }
        break;
    case 4:
        for (size_t d3 = 0; d3 < a.shape(3); d3++)
        {
            for (size_t d2 = 0; d2 < a.shape(2); d2++)
            {
                std::cout << "val(:,:," << d2 + b << "," << d3 + b << ") = " << std::endl
                          << xt::print_options::precision(16) << xt::view(a, xt::all(), xt::all(), d2, d3) << std::endl
                          << std::endl;
            }
        }
        break;
    default:
        std::cout << xt::print_options::precision(16) << a << std::endl
                  << std::endl;
        break;
    }
}
void pb(xt::xarray<bool> &a, bool b)
{
    pa<bool>(a, b);
}
void ps(xt::xarray<size_t> &a, bool b)
{
    pa<size_t>(a, b);
}
void pi(xt::xarray<int> &a, bool b)
{
    pa<int>(a, b);
}
void pd(xt::xarray<double> &a, bool b)
{
    pa<double>(a, b);
}