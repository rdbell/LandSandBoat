#include "test_treasure_hunter_proc_1535.h"

#include "map/treasure_hunter_proc_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using treasurehunterprochelpers::AutoUpgradeMobTH;
using treasurehunterprochelpers::CanUpgradeTreasureHunter;
using treasurehunterprochelpers::ResolveTreasureHunterProc;
using treasurehunterprochelpers::ShouldEvaluateTreasureHunterProc;
using treasurehunterprochelpers::TreasureHunterProcRate;

auto Check() -> bool
{
    if (!ShouldEvaluateTreasureHunterProc(true, false, 10, true, true, true, true) ||
        ShouldEvaluateTreasureHunterProc(true, true, 10, true, true, true, true))
    {
        return false;
    }
    if (!CanUpgradeTreasureHunter(10, 0) || CanUpgradeTreasureHunter(12, 0))
    {
        return false;
    }
    {
        std::int16_t newTH = 0;
        if (!AutoUpgradeMobTH(5, 10, newTH) || newTH != 8)
        {
            return false;
        }
    }
    if (std::fabs(TreasureHunterProcRate(8, 8, 0, 0, false, false) - 0.04f) > 1e-6f)
    {
        return false;
    }
    if (std::fabs(TreasureHunterProcRate(10, 8, 0, 0, false, false) - 0.01f) > 1e-6f)
    {
        return false;
    }
    if (std::fabs(TreasureHunterProcRate(8, 8, 0, 0, true, false) - 0.4f) > 1e-6f)
    {
        return false;
    }
    {
        const auto r = ResolveTreasureHunterProc(12, 10, 0, 0, 0, false, false, 0.0f);
        if (r.eligible)
        {
            return false;
        }
    }
    {
        const auto r = ResolveTreasureHunterProc(8, 8, 0, 0, 0, true, true, 0.0f);
        if (!r.eligible || !r.procced)
        {
            return false;
        }
    }
    {
        const auto r = ResolveTreasureHunterProc(8, 8, 0, 0, 0, false, false, 0.05f);
        if (!r.eligible || r.procced)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runTreasureHunterProc1535SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "treasure_hunter_proc_1535 self-tests failed\n";
        return false;
    }
    return true;
}
