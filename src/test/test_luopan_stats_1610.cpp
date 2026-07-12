#include "test_luopan_stats_1610.h"

#include "map/luopan_stats_capacity.h"

#include <iostream>

namespace
{
using namespace luopanstatshelpers;

auto Check() -> bool
{
    if (BaseMaxHP(1) != 16 || BaseMaxHP(15) != 250 || BaseMaxHP(75) != 1250 || BaseMaxHP(99) != 1650 || BaseMaxHP(0) != 0)
    {
        return false;
    }
    if (BolsterMaxHPBonus(1250, 5) != 187 || BolsterMaxHPBonus(1250, 0) != 0 || BolsterMaxHPBonus(16, 1) != 0)
    {
        return false;
    }
    if (MaxHP(75, false, 5) != 1250 || MaxHP(75, true, 5) != 1250 + 187)
    {
        return false;
    }
    if (EntityFlags != 0x0000008BU || BaseSpeed != 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runLuopanStats1610SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "luopan_stats_1610 self-tests failed\n";
        return false;
    }
    return true;
}
