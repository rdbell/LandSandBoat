#include "test_scaled_item_modifier_1545.h"

#include "map/scaled_item_modifier_capacity.h"

#include <iostream>

namespace
{
using scaleditemmodifierhelpers::ScaledItemModifier;
using scaleditemmodifierhelpers::ShouldRejectNull;

auto Check() -> bool
{
    if (!ShouldRejectNull(true, false) || !ShouldRejectNull(false, true) || ShouldRejectNull(false, false))
    {
        return false;
    }
    // At/above req: passthrough
    if (ScaledItemModifier(75, 75, Mod::STR, 15) != 15 || ScaledItemModifier(99, 75, Mod::DEF, 40) != 40)
    {
        return false;
    }
    if (ScaledItemModifier(1, 0, Mod::TP_BONUS, 500) != 500)
    {
        return false;
    }
    // DEF family under-level: amount*3/4 / req
    // 300*3/4=225, 225/75=3
    if (ScaledItemModifier(50, 75, Mod::DEF, 300) != 3)
    {
        return false;
    }
    // 40*3/4=30, 30/75=0
    if (ScaledItemModifier(50, 75, Mod::DEF, 40) != 0)
    {
        return false;
    }
    // HP: 100/2/50=1
    if (ScaledItemModifier(1, 50, Mod::HP, 100) != 1)
    {
        return false;
    }
    // Stats: 300/3/50=2
    if (ScaledItemModifier(1, 50, Mod::STR, 300) != 2)
    {
        return false;
    }
    // Default under-level → 0
    if (ScaledItemModifier(1, 99, Mod::TP_BONUS, 500) != 0)
    {
        return false;
    }
    // MAIN/SUB/RANGED dmg ratings
    if (ScaledItemModifier(1, 50, Mod::SUB_DMG_RATING, 400) != 6) // 300/50
    {
        return false;
    }
    return true;
}
} // namespace

auto runScaledItemModifier1545SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "scaled_item_modifier_1545 self-tests failed\n";
        return false;
    }
    return true;
}
