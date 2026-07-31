#include "test_crit_hit_rate_1552.h"

#include "map/crit_hit_rate_capacity.h"

#include <iostream>

namespace
{
using crithitratehelpers::AgiCritBonus;
using crithitratehelpers::ClassifyMeleeCritPath;
using crithitratehelpers::DexCritBonus;
using crithitratehelpers::FencerCritEligible;
using crithitratehelpers::MeleeCritHitRate;
using crithitratehelpers::MeleeCritPath;
using crithitratehelpers::RangedCritHitRate;

auto Check() -> bool
{
    // DexCritBonus table samples
    if (DexCritBonus(10, 10) != 0 || DexCritBonus(17, 10) != 1 || DexCritBonus(25, 10) != 2)
    {
        return false;
    }
    if (DexCritBonus(35, 10) != 3 || DexCritBonus(45, 10) != 4 || DexCritBonus(50, 10) != 5)
    {
        return false;
    }
    // clamp 50 max dDEX: attacker 100 defender 10 → dDEX 50 → 15
    if (DexCritBonus(100, 10) != 15)
    {
        return false;
    }
    // AgiCritBonus: +1 per 10 positive dAGI
    if (AgiCritBonus(50, 50) != 0 || AgiCritBonus(60, 50) != 1 || AgiCritBonus(99, 50) != 4)
    {
        return false;
    }
    if (AgiCritBonus(40, 50) != 0)
    {
        return false;
    }

    // Melee assembly: base 5 + merit 5 + fencer 3 - enemy 2 + innin 10 - yonin 5 + dex 4 + mod 15 - eva 3 + wep 2
    // = 5+5+3-2+10-5+4+15-3+2 = 34
    if (MeleeCritHitRate(5, 3, 2, 10, 5, 4, 15, 3, 2) != 34)
    {
        return false;
    }
    // clamp high
    if (MeleeCritHitRate(25, 20, 0, 30, 0, 15, 50, 0, 10) != 100)
    {
        return false;
    }
    // clamp low
    if (MeleeCritHitRate(0, 0, 20, 0, 30, 0, 0, 50, 0) != 0)
    {
        return false;
    }

    // Ranged: base 5 + merit 5 - enemy 2 + innin 10 - yonin 5 + (mod 20 - MS 100) + agi 3 - eva 1
    // = 5+5-2+10-5+20-100+3-1 = -65 → 0
    if (RangedCritHitRate(5, 2, 10, 5, 20, true, 3, 1) != 0)
    {
        return false;
    }
    // Ranged without MS: 5+5+10+20+3 = 43
    if (RangedCritHitRate(5, 0, 10, 0, 20, false, 3, 0) != 43)
    {
        return false;
    }

    // Path classification
    if (ClassifyMeleeCritPath(true, false, true, true, false, false, false, false) != MeleeCritPath::Forced100)
    {
        return false;
    }
    if (ClassifyMeleeCritPath(true, false, true, false, true, true, true, true) != MeleeCritPath::BareFive)
    {
        return false;
    }
    if (ClassifyMeleeCritPath(true, false, false, false, true, true, true, true) != MeleeCritPath::Forced100)
    {
        return false;
    }
    if (ClassifyMeleeCritPath(true, true, true, true, true, true, true, true) != MeleeCritPath::Assembly)
    {
        return false;
    }

    // Fencer eligible: one-handed main with no sub-item.
    if (!FencerCritEligible(true, false, false, false, false, 0, false))
    {
        return false;
    }
    // Main-hand and sub-item gates.
    if (FencerCritEligible(false, false, false, false, false, 0, false) ||
        FencerCritEligible(true, true, false, false, false, 0, false) ||
        FencerCritEligible(true, false, true, false, false, 0, false))
    {
        return false;
    }
    // A shield qualifies; a weapon only qualifies when its skill type is NONE.
    if (!FencerCritEligible(true, false, false, true, false, 32, true) ||
        !FencerCritEligible(true, false, false, true, true, 0, false) ||
        FencerCritEligible(true, false, false, true, true, 3, false) ||
        FencerCritEligible(true, false, false, true, false, 0, false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCritHitRate1552SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "crit_hit_rate_1552 self-tests failed\n";
        return false;
    }
    return true;
}
