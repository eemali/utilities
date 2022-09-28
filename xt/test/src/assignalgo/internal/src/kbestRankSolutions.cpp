#include "../include/kbestRankSolutions.h"
#include <algorithm>
namespace assignalgo
{
    void kbestRankSolutions(std::deque<solution_t> &solutionList)
    {
        std::stable_sort(solutionList.begin(), solutionList.end(),
                         [](const solution_t &s1, const solution_t &s2) {
                             return std::get<2>(s1)[0] < std::get<2>(s2)[0];
                         });
    }
}