
#include "../include/lapCheckUnassignedCost.h"
#include <cmath>
#include <stdexcept> // for std::runtime_error
namespace assignalgo
{
    void lapCheckUnassignedCost(const double &costOfNonAssignment)
    {
        if (!std::isfinite(costOfNonAssignment))
        {
            std::runtime_error("costOfNonAssignment must be finite");
        }
    }
}