#include "test_trust_battlefield_1584.h"

#include "map/trust_battlefield_capacity.h"

#include <iostream>

namespace
{
using namespace trustbattlefieldhelpers;

auto Check() -> bool
{
    if (!IsRoVKIBattlefield(5) || !IsRoVKIBattlefield(1154) || IsRoVKIBattlefield(1))
    {
        return false;
    }
    if (!CheckBattlefieldTrustCount(false, 1, 6, 5, false, false))
    {
        return false;
    }
    if (!CheckBattlefieldTrustCount(true, 6, 3, 2, false, false))
    {
        return false;
    }
    if (CheckBattlefieldTrustCount(true, 6, 3, 3, false, false))
    {
        return false;
    }
    // RoV KI without umber keeps max=1 → full.
    if (CheckBattlefieldTrustCount(true, 1, 1, 0, true, false))
    {
        return false;
    }
    // RoV KI with umber raises max to 6.
    if (!CheckBattlefieldTrustCount(true, 1, 1, 0, true, true))
    {
        return false;
    }
    if (CheckBattlefieldTrustCount(true, 1, 1, 5, true, true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runTrustBattlefield1584SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "trust_battlefield_1584 self-tests failed\n";
        return false;
    }
    return true;
}
