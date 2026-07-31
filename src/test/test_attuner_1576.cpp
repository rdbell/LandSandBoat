#include "test_attuner_1576.h"

#include "map/attuner_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace attunerhelpers;

auto AlmostEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

auto Check() -> bool
{
    if (AttunerAttachmentSlot() != 5)
    {
        return false;
    }
    if (ManeuverCount(0, true) != 0 || ManeuverCount(2, false) != 2 || ManeuverCount(2, true) != 3 || ManeuverCount(5, false) != 3)
    {
        return false;
    }
    if (!AlmostEqual(AttunerDefIgnore(false, true, true, 50, 75, 3, false), 0.0))
    {
        return false;
    }
    if (!AlmostEqual(AttunerDefIgnore(true, true, true, 50, 75, 0, false), 0.05))
    {
        return false;
    }
    if (!AlmostEqual(AttunerDefIgnore(true, true, true, 50, 75, 3, false), 0.20))
    {
        return false;
    }
    if (!AlmostEqual(AttunerDefIgnore(true, true, true, 50, 75, 1, true), 0.20))
    {
        return false;
    }
    if (!AlmostEqual(AttunerDefIgnore(true, true, true, 75, 50, 3, false), 0.0))
    {
        return false;
    }
    return true;
}
} // namespace

auto runAttuner1576SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "attuner_1576 self-tests failed\n";
        return false;
    }
    return true;
}
