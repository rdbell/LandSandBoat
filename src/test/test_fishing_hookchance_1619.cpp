#include "test_fishing_hookchance_1619.h"

#include "map/fishing_hookchance_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace fishinghookchancehelpers;

auto Near(const float a, const float b, const float eps = 1e-5f) -> bool
{
    return std::fabs(a - b) <= eps;
}

auto Check() -> bool
{
    // Clamp01 bounds pattern values to the unit interval.
    if (Clamp01(-0.25f) != 0.0f || Clamp01(0.0f) != 0.0f || Clamp01(0.625f) != 0.625f ||
        Clamp01(1.0f) != 1.0f || Clamp01(1.25f) != 1.0f)
    {
        return false;
    }

    // Moon phase classifier
    if (GetMoonPhase(0, 0) != 0 || GetMoonPhase(95, 0) != 4 || GetMoonPhase(20, 2) != 1)
    {
        return false;
    }
    if (GetMoonPhase(50, 1) != 6 || GetMoonPhase(20, 1) != 7)
    {
        return false;
    }

    // Monthly default +0.25 → 0.75
    if (!Near(GetMonthlyTidalInfluence(0, 0), 0.75f))
    {
        return false;
    }

    // Hourly pattern 2 peak everywhere except 5/17
    if (!Near(GetHourlyModifier(2, 12), 1.25f) || !Near(GetHourlyModifier(2, 5), 0.75f))
    {
        return false;
    }

    // Moon pattern 0 default → 1.0 + 0.25 = 1.25
    if (!Near(GetMoonModifier(0, 0), 1.25f))
    {
        return false;
    }
    // case 5 uses pattern 4 (not 5)
    if (!Near(GetMoonModifier(5, 0), GetMoonModifier(4, 0)))
    {
        return false;
    }

    // Lucky moon: 1 + floor(MoonPattern1 * 3)
    {
        const auto lucky = GetLuckyMoonModifierFromPhase(0);
        if (lucky < 1 || lucky > 4)
        {
            return false;
        }
    }

    // Hook chance: all mods 1.0 → (1+2+3)/3=2 → floor(50)=50; no bait/penalty → clamp 50
    // monthMod=1, hourMod=1, moonMod=1 → hour*2=2, moon*3=3, avg=2, floor(50)=50
    if (CalculateHookChance(50, 50, 0, 1000, false, 0, false, false, true, 0, 1.0f, 1.0f, 1.0f) != 50)
    {
        return false;
    }
    // bait power 1 lure +30 → 80
    if (CalculateHookChance(50, 50, 0, 1000, false, 1, true, false, true, 0, 1.0f, 1.0f, 1.0f) != 80)
    {
        return false;
    }
    // floor clamp 20
    if (CalculateHookChance(50, 50, 0, 1000, false, 0, false, false, true, 0, 0.0f, 0.0f, 0.0f) != 20)
    {
        return false;
    }
    // reverse skill penalty: skill 100 fish 50 → pen floor((100-10-50)*0.15)=floor(6)=6 → 50-6=44
    if (CalculateHookChance(100, 50, 0, 1000, false, 0, false, false, true, 0, 1.0f, 1.0f, 1.0f) != 44)
    {
        return false;
    }
    // shellfish +50
    if (CalculateHookChance(50, 50, 0, 1000, true, 0, false, true, true, 0, 1.0f, 1.0f, 1.0f) != 100)
    {
        return false;
    }
    // rarity 500 → floor(50 * 0.5)=25
    if (CalculateHookChance(50, 50, 0, 500, false, 0, false, false, true, 0, 1.0f, 1.0f, 1.0f) != 25)
    {
        return false;
    }
    // non-legendary rod size fish small rod large → -3
    if (CalculateHookChance(50, 50, 0, 1000, false, 0, false, false, false, 1, 1.0f, 1.0f, 1.0f) != 47)
    {
        return false;
    }

    return true;
}
} // namespace

auto runFishingHookChance1619SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fishing_hookchance_1619 self-tests failed\n";
        return false;
    }
    return true;
}
