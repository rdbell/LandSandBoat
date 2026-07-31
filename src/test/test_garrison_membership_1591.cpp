#include "test_garrison_membership_1591.h"

#include "map/garrison_membership_capacity.h"

#include <iostream>
#include <vector>

namespace
{
using namespace garrisonmembershiphelpers;

auto Check() -> bool
{
    ClearAllZonePlayers();

    const std::vector<std::uint32_t> players{ 10, 20, 30 };
    if (!ContainsPlayerID(20, players) || ContainsPlayerID(99, players) || ContainsPlayerID(1, {}))
    {
        return false;
    }
    if (!IsInGarrison(100, 20, true, players) || IsInGarrison(100, 99, true, players) ||
        IsInGarrison(0, 20, true, players) || IsInGarrison(100, 20, false, players))
    {
        return false;
    }

    // Host registry
    SetZonePlayers(50, players);
    if (!IsPlayerInGarrison(50, 20) || IsPlayerInGarrison(50, 99) || IsPlayerInGarrison(51, 20))
    {
        return false;
    }
    ClearZonePlayers(50);
    if (IsPlayerInGarrison(50, 20))
    {
        return false;
    }

    // Replace roster
    SetZonePlayers(7, { 1, 2 });
    SetZonePlayers(7, { 3 });
    if (IsPlayerInGarrison(7, 1) || !IsPlayerInGarrison(7, 3))
    {
        return false;
    }

    // Clearing all zones must invalidate every active roster, not only the
    // most recently updated zone.
    SetZonePlayers(8, { 4 });
    ClearAllZonePlayers();
    if (IsPlayerInGarrison(7, 3) || IsPlayerInGarrison(8, 4))
    {
        return false;
    }

    return true;
}
} // namespace

auto runGarrisonMembership1591SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "garrison_membership_1591 self-tests failed\n";
        return false;
    }
    return true;
}
