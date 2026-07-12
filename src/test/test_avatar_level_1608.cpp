#include "test_avatar_level_1608.h"

#include "map/avatar_stats_capacity.h"

#include <iostream>

namespace
{
using namespace avatarstatshelpers;

auto Check() -> bool
{
    if (PetSpecificLevelBonus(PetIDCarbuncle, 3, 5) != 3 || PetSpecificLevelBonus(PetIDCaitSith, 3, 5) != 5 ||
        PetSpecificLevelBonus(10, 3, 5) != 0)
    {
        return false;
    }
    if (MainJobLevel(75, 2, 1) != 78 || MainJobLevel(1, -5, 0) != 0)
    {
        return false;
    }
    if (ResolveLevel(true, false, 75, 37, 2, 1) != 78)
    {
        return false;
    }
    if (ResolveLevel(false, true, 75, 37, 2, 1) != 37)
    {
        return false;
    }
    if (ResolveLevel(false, false, 75, 37, 2, 1) != FallbackLevel)
    {
        return false;
    }
    if (ResolveLevel(true, true, 99, 49, 0, 0) != 99)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAvatarLevel1608SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "avatar_level_1608 self-tests failed\n";
        return false;
    }
    return true;
}
