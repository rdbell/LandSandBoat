#include "test_mob_base_1598.h"

#include "map/mob_base_capacity.h"

#include <iostream>

namespace
{
using namespace mobbasehelpers;

auto Check() -> bool
{
    // GetBaseDefEva pins (match internal/mobutils def_eva_test samples)
    if (GetBaseDefEva(1, 1) != 6 || GetBaseDefEva(1, 50) != 6 + 49 * 3)
    {
        return false;
    }
    // L51 rank A: 153 + 5 = 158
    if (GetBaseDefEva(1, 51) != 158)
    {
        return false;
    }
    // L75 rank A: 153 + 25*5 = 278
    if (GetBaseDefEva(1, 75) != 278)
    {
        return false;
    }
    if (GetBaseDefEva(0, 50) != 0 || GetBaseDefEva(6, 50) != 0)
    {
        return false;
    }

    // Subjob zones
    if (!IsSubJobZone(100) || !IsSubJobZone(220) || IsSubJobZone(0) || IsSubJobZone(256))
    {
        return false;
    }
    int n = 0;
    for (std::uint16_t z = 1; z < 300; ++z)
    {
        if (IsSubJobZone(z))
        {
            ++n;
        }
    }
    if (n != 94)
    {
        std::cerr << "IsSubJobZone count = " << n << " want 94\n";
        return false;
    }
    return true;
}
} // namespace

auto runMobBase1598SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "mob_base_1598 self-tests failed\n";
        return false;
    }
    return true;
}
