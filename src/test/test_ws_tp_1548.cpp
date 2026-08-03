#include "test_ws_tp_1548.h"

#include "map/ws_tp_capacity.h"

#include <iostream>

namespace
{
using wstphelpers::CalculateWeaponSkillTP;
using wstphelpers::FencerEligible;
using wstphelpers::IsRangedWeaponskillID;
using wstphelpers::MaxWeaponskillTP;

auto Check() -> bool
{
    if (!IsRangedWeaponskillID(192) || !IsRangedWeaponskillID(221) || IsRangedWeaponskillID(191) || IsRangedWeaponskillID(222))
    {
        return false;
    }
    // Non-PC: spent + bonus, cap 3000
    if (CalculateWeaponSkillTP(1000, 100, false, false, 0, 0, 0, false, 0) != 1100)
    {
        return false;
    }
    if (CalculateWeaponSkillTP(2500, 1000, false, false, 0, 0, 0, false, 0) != MaxWeaponskillTP)
    {
        return false;
    }
    // PC melee: subtract sub + ranged, add fencer
    // 1000+500 - 50 - 100 + 80 = 1430
    if (CalculateWeaponSkillTP(1000, 500, true, false, 50, 0, 100, true, 80) != 1430)
    {
        return false;
    }
    // PC ranged: subtract sub + main, no fencer
    // 1000+500 - 50 - 200 = 1250
    if (CalculateWeaponSkillTP(1000, 500, true, true, 50, 200, 999, true, 80) != 1250)
    {
        return false;
    }
    // LSB preserves negative intermediate TP rather than flooring at zero.
    if (CalculateWeaponSkillTP(100, 0, true, false, 250, 0, 0, false, 0) != -150)
    {
        return false;
    }
    if (!FencerEligible(true, false, false, false, false, 0, false))
    {
        return false;
    }
    if (FencerEligible(false, false, false, false, false, 0, false))
    {
        return false; // no main-hand weapon
    }
    if (FencerEligible(true, true, false, false, false, 0, false))
    {
        return false; // two-handed
    }
    if (FencerEligible(true, false, true, false, false, 0, false))
    {
        return false; // hand-to-hand
    }
    if (!FencerEligible(true, false, false, true, false, 0, true))
    {
        return false; // shield
    }
    if (!FencerEligible(true, false, false, true, true, 0, false))
    {
        return false; // grip SKILL_NONE
    }
    if (FencerEligible(true, false, false, true, true, 1, false))
    {
        return false; // real offhand weapon
    }
    if (FencerEligible(true, false, false, true, false, 0, false))
    {
        return false; // non-weapon, non-shield offhand
    }
    return true;
}
} // namespace

auto runWSTP1548SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "ws_tp_1548 self-tests failed\n";
        return false;
    }
    return true;
}
