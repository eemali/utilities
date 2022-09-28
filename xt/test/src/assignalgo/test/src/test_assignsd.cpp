#include "../../include/assignsd.h"
#include "../../include/assignjv.h"
#include "../../include/assignmunkres.h"
#include "../../include/assignmunkres_fast.h"
#include <xtensor/xio.hpp>
#include <xtensor/xnorm.hpp>
#include <xtensor/xrandom.hpp>
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
int main(int argc, char const *argv[])
{
	double inf = std::numeric_limits<double>::infinity();
#if 0
	xt::random::seed(2021);
	xt::xarray<double> costMatrix = xt::random::rand<double>({ 6, 6, 6 });
#else
	xt::xarray<double> costMatrix = {{{0.605978278807405, 0.178993105950188, 0.962395990210392, 0.574304294454671, 0.583704016323978, 0.672128935096512},
									  {0.733369361193498, 0.752925428776094, 0.616557443894574, 0.371160848157782, 0.476313473592116, 0.643484806343680},
									  {0.138947156728399, 0.662160514309534, 0.0866299612485472, 0.452145243789564, 0.517814400301174, 0.828014370259165},
									  {0.312673083854687, 0.784310132141123, 0.561272362500197, 0.201850247834620, 0.823098633965347, 0.204469394354834},
									  {0.997243281340319, 0.0968943959248608, 0.616524708617990, 0.569305118050573, 0.732225026938991, 0.617488953431101},
									  {0.128162375418961, 0.0585712850880360, 0.963843022875632, 0.195095974342115, 0.0690562748249788, 0.617701012022227}},
									 {{0.301068620764161, 0.508830900547894, 0.636829924554549, 0.265164948613886, 0.0638364988604113, 0.0908669014763180},
									  {0.871740585603846, 0.431786183797688, 0.342208937023058, 0.650923754451574, 0.414441178253752, 0.874556821592078},
									  {0.589654082067040, 0.915939563972423, 0.823791182791286, 0.154614808420178, 0.332244248737148, 0.776976862335028},
									  {0.981770092591056, 0.709015642422061, 0.305150772122459, 0.388699499046528, 0.721320975425436, 0.771566418264295},
									  {0.442232233509664, 0.890655387080499, 0.307543465351416, 0.647614143502209, 0.473847301678784, 0.0631033305913746},
									  {0.126317685562099, 0.588885607861431, 0.201322662261775, 0.848299115427671, 0.798641336787792, 0.594438483359758}},
									 {{0.792494733115708, 0.417257234462832, 0.970198789207506, 0.823277909887818, 0.648545479366243, 0.452418129778481},
									  {0.274133677753527, 0.320898734566101, 0.415507041608741, 0.556910443833738, 0.343326612861967, 0.117499654518431},
									  {0.371019812991847, 0.372296369027184, 0.330489067487834, 0.718501026164313, 0.622637319501393, 0.820002026948333},
									  {0.853114542270778, 0.275352176970130, 0.966714162457221, 0.879936921794659, 0.464599661570199, 0.175130196630558},
									  {0.678728918932166, 0.679555601487995, 0.825899042971108, 0.496010051968121, 0.123839956780077, 0.230698196397888},
									  {0.754377463959394, 0.581626445482728, 0.305400183440637, 0.512086310323716, 0.980120858073161, 0.704817426757157}},
									 {{0.717647706215276, 0.857989447847609, 0.266679886318974, 0.210963667532510, 0.590696683042375, 0.905414249805280},
									  {0.148248295156759, 0.296740243703242, 0.382407052524053, 0.674073455105263, 0.854534108125375, 0.0801849192393843},
									  {0.393682256118828, 0.158843626866391, 0.503558471535750, 0.897229560663834, 0.632654996731809, 0.680495282025248},
									  {0.802618114509650, 0.686143109205511, 0.657162787628565, 0.762114898430710, 0.868411705608245, 0.0833892903960918},
									  {0.810188631273749, 0.733545227584694, 0.751924262958477, 0.816432186896560, 0.416419606011566, 0.208987131269810},
									  {0.796002798990539, 0.148005677830076, 0.0674818882891828, 0.609532098945787, 0.657412921317226, 0.838315395141101}},
									 {{0.522378556804680, 0.795407360352229, 0.736258815582551, 0.417461324337300, 0.940165467483231, 0.607031982025110},
									  {0.721429256142813, 0.0219511474820985, 0.127722215994192, 0.871172529643581, 0.0704853899680313, 0.401780871733983},
									  {0.862353104806728, 0.407489503453794, 0.609540916523668, 0.761974259050562, 0.655953871848279, 0.303725822612129},
									  {0.768818681954348, 0.945650608087895, 0.0623242818208980, 0.0186023741507042, 0.379713504158798, 0.671453258733748},
									  {0.677331512582485, 0.944483831905629, 0.597233816245972, 0.743562180994457, 0.424859037804165, 0.587258567579369},
									  {0.555212797223849, 0.615106340118526, 0.428251563867324, 0.895972920422044, 0.536689982616545, 0.240006673160679}},
									 {{0.483364039565941, 0.699376914747415, 0.00503950468755232, 0.271760826985707, 0.875604298195070, 0.0293920736220161},
									  {0.192552572535364, 0.595775698244697, 0.431993045387554, 0.629684262345404, 0.262749549335637, 0.934147308168381},
									  {0.994023259191578, 0.738506330679825, 0.894026570206815, 0.00872070020563753, 0.652286250662780, 0.0905884419245437},
									  {0.267721333798895, 0.749052123090778, 0.756157156898089, 0.168139594428483, 0.270055959927292, 0.878635367898622},
									  {0.791408012706047, 0.292431866111675, 0.337336194088390, 0.463522342337281, 0.248056992708217, 0.313588232936073},
									  {0.448188631251073, 0.342872744525137, 0.0840321563817479, 0.934771168247054, 0.628052841014610, 0.379720137931509}}};
	costMatrix = xt::transpose(costMatrix, {1, 2, 0});
#endif // 1
	xt::xarray<double> fullMatrix = xt::xarray<double>({7, 7, 7}, inf);
	xt::view(fullMatrix, xt::range(1, _), xt::range(1, _), xt::range(1, _)) = costMatrix;
	fullMatrix.at(0, 0, 0) = 0;

	std::cout << costMatrix << std::endl;
	{
		xt::xarray<size_t> assignments;
		double costOfAssignment, solutionGap;

		auto start = std::chrono::high_resolution_clock::now();
		std::tie(assignments, costOfAssignment, solutionGap) = assignalgo::assignsd(fullMatrix, 0.01, 100, &assignalgo::assignauction);
		auto stop = std::chrono::high_resolution_clock::now();

		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() * 1e-9;
		std::cout << "assignsd with algFcn = assignauction" << std::endl;
		std::cout << assignments << std::endl;
		std::cout << "costOfAssignment: " << costOfAssignment << std::endl;
		std::cout << "solutionGap: " << solutionGap << std::endl;
		std::cout << "Time taken: "
				  << std::fixed << std::setprecision(9)
				  << duration << " s" << std::endl
				  << std::endl;
	}
	{
		xt::xarray<size_t> assignments;
		double costOfAssignment, solutionGap;

		auto start = std::chrono::high_resolution_clock::now();
		std::tie(assignments, costOfAssignment, solutionGap) = assignalgo::assignsd(fullMatrix, 0.01, 100, &assignalgo::assignjv);
		auto stop = std::chrono::high_resolution_clock::now();

		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() * 1e-9;
		std::cout << "assignsd with algFcn = assignjv" << std::endl;
		std::cout << assignments << std::endl;
		std::cout << "costOfAssignment: " << costOfAssignment << std::endl;
		std::cout << "solutionGap: " << solutionGap << std::endl;
		std::cout << "Time taken: "
				  << std::fixed << std::setprecision(9)
				  << duration << " s" << std::endl
				  << std::endl;
	}
	{
		xt::xarray<size_t> assignments;
		double costOfAssignment, solutionGap;

		auto start = std::chrono::high_resolution_clock::now();
		std::tie(assignments, costOfAssignment, solutionGap) = assignalgo::assignsd(fullMatrix, 0.01, 100, &assignalgo::assignmunkres);
		auto stop = std::chrono::high_resolution_clock::now();

		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() * 1e-9;
		std::cout << "assignsd with algFcn = assignmunkres" << std::endl;
		std::cout << assignments << std::endl;
		std::cout << "costOfAssignment: " << costOfAssignment << std::endl;
		std::cout << "solutionGap: " << solutionGap << std::endl;
		std::cout << "Time taken: "
				  << std::fixed << std::setprecision(9)
				  << duration << " s" << std::endl
				  << std::endl;
	}
	{
		xt::xarray<size_t> assignments;
		double costOfAssignment, solutionGap;

		auto start = std::chrono::high_resolution_clock::now();
		std::tie(assignments, costOfAssignment, solutionGap) = assignalgo::assignsd(fullMatrix, 0.01, 100, &assignalgo::assignmunkres_fast);
		auto stop = std::chrono::high_resolution_clock::now();

		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() * 1e-9;
		std::cout << "assignsd with algFcn = assignmunkres_fast" << std::endl;
		std::cout << assignments << std::endl;
		std::cout << "costOfAssignment: " << costOfAssignment << std::endl;
		std::cout << "solutionGap: " << solutionGap << std::endl;
		std::cout << "Time taken: "
				  << std::fixed << std::setprecision(9)
				  << duration << " s" << std::endl
				  << std::endl;
	}
	return 0;
}
