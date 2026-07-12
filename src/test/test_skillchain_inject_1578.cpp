#include "test_skillchain_inject_1578.h"

#include "map/skillchain_inject_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace skillchaininjecthelpers;

auto AlmostEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

auto Check() -> bool
{
    if (!AlmostEqual(StaffBonus(0, 5), 1.0) || !AlmostEqual(StaffBonus(1, 2), 1.10))
    {
        return false;
    }
    if (!AlmostEqual(AffinityBonus(0, 50), 1.0) || !AlmostEqual(AffinityBonus(1, 25), 1.25))
    {
        return false;
    }
    if (NullificationFactor(true, false, false) != 0.0 || NullificationFactor(false, false, false) != 1.0)
    {
        return false;
    }
    if (AbsorptionFactor(1.0, true, false, false) != -1.0 || AbsorptionFactor(-0.5, false, false, false) != -0.5)
    {
        return false;
    }
    // Magical DT: no mods → 1.0
    if (!AlmostEqual(MagicalDamageAdjustment(0, 0, 0, 0), 1.0))
    {
        return false;
    }
    // Day/weather identity
    DayWeatherParams d{};
    d.spellElement = 1;
    d.weather      = 0;
    d.dayElement   = 0;
    if (!AlmostEqual(DayWeatherMultiplier(d), 1.0))
    {
        return false;
    }
    // Force apply + matching hot spell weather
    d.alwaysApply = true;
    d.weather     = 4; // HotSpell
    if (!AlmostEqual(DayWeatherMultiplier(d), 1.10))
    {
        return false;
    }
    if (!RollProc1to100(50, 50) || RollProc1to100(51, 50) || RollProc1to100(1, 0))
    {
        return false;
    }
    return true;
}
} // namespace

auto runSkillchainInject1578SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "skillchain_inject_1578 self-tests failed\n";
        return false;
    }
    return true;
}
