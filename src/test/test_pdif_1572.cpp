#include "test_pdif_1572.h"

#include "map/pdif_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace pdifhelpers;

auto AlmostEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

auto Check() -> bool
{
    // Weapon caps
    if (!AlmostEqual(WeaponCap(1), 3.5) || !AlmostEqual(WeaponCap(7), 4.0) || !AlmostEqual(WeaponCap(99), DefaultWeaponCap))
    {
        return false;
    }

    // Level dif
    if (!AlmostEqual(LevelDifFactor(60, 50, true, true, false), 0.0))
    {
        return false;
    }
    if (!AlmostEqual(LevelDifFactor(50, 60, true, true, false), -10.0 * MeleeLevelCorrectionPerLevel))
    {
        return false;
    }

    // Spike PC mid
    if (SpikeRatio(true, 1.0) <= 0.0 || SpikeRatio(true, 0.5) != 0.0 || SpikeRatio(true, 1.5) != 0.0)
    {
        return false;
    }

    // Melee bounds + spike
    {
        double lower = 0;
        double upper = 0;
        bool   spike = false;
        MeleeBounds(100, 100, false, false, true, 75, 75, 3.25, 0, 0, 1, 1, lower, upper, spike);
        if (!spike)
        {
            return false; // roll 1 should spike at wRatio 1.0 for PC
        }
        MeleeBounds(100, 100, false, false, true, 75, 75, 3.25, 0, 0, 10000, 1, lower, upper, spike);
        if (spike || upper <= 0.0)
        {
            return false;
        }
    }

    // Finish melee / ranged
    if (!AlmostEqual(FinishMeleePDIF(1.5, 0, false, 0, 0), 1.5))
    {
        return false;
    }
    if (!AlmostEqual(FinishMeleePDIF(1.5, 5, false, 0, 0), 1.5 * 1.05))
    {
        return false;
    }
    if (!AlmostEqual(FinishRangedPDIF(1.0, true, 0, 0), 1.25))
    {
        return false;
    }

    // Crit damage mult
    if (!AlmostEqual(CritDamageMult(20, 5), 1.15) || !AlmostEqual(CritDamageMult(200, 0), 2.0))
    {
        return false;
    }

    // Effective defense
    if (!AlmostEqual(EffectiveDefense(100, true, 0.25), 75.0) || !AlmostEqual(EffectiveDefense(100, false, 0.5), 100.0))
    {
        return false;
    }

    return true;
}
} // namespace

auto runPDIF1572SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "pdif_1572 self-tests failed\n";
        return false;
    }
    return true;
}
