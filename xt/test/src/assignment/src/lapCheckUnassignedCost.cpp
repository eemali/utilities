
#include "../include/lapCheckUnassignedCost.h"
#include <cassert>
#include <cmath>
namespace assignalgo
{
    void lapCheckUnassignedCost(const double &costOfNonAssignment)
    {
        assert(std::isfinite(costOfNonAssignment));
    }
}