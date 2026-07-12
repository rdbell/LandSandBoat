#include "test_trust_stats_1611.h"

#include "map/trust_stats_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace truststatshelpers;

auto nearly(const float a, const float b) -> bool
{
    return std::fabs(a - b) < 1e-4f;
}

auto Check() -> bool
{
    if (ScaleToGrade(1.0f) != 4 || ScaleToGrade(0.7f) != 6 || ScaleToGrade(1.4f) != 1)
    {
        return false;
    }
    if (!nearly(ClampAlterEgoMultiplier(1.5f), 1.5f) || !nearly(ClampAlterEgoMultiplier(0.05f), 1.0f) || !nearly(ClampAlterEgoMultiplier(3.0f), 1.0f))
    {
        return false;
    }
    if (SkillCapLevel(50) != 50 || SkillCapLevel(99) != 99 || SkillCapLevel(100) != 99)
    {
        return false;
    }
    if (ScaleSubJobStat(20, 16) != 10 || ScaleSubJobStat(20, 15) != 0 || ScaleSubJobStat(20, 1) != 0)
    {
        return false;
    }
    if (ApplySkillMultiplier(100, 1.5f) != 150 || FinalTrustStat(10, 10, 5, 1.0f) != 25)
    {
        return false;
    }
    if (FinalTrustMaxHP(100, 200, 30, 50, 1.0f) != 380 || FinalTrustMaxMP(10, 20, 5, 2.0f) != 70)
    {
        return false;
    }
    // Trust sjob HP: 10 + 1*14 + 0.5*5 + 5 + 10 = 41.5 (no /2)
    if (!nearly(ComposeTrustSubJobHP(10, 1, 0.5f, 15, 5, 10), 41.5f))
    {
        return false;
    }
    if (!nearly(ComposeTrustSubJobMP(5, 2), 7.0f))
    {
        return false;
    }
    if (ExpoHPP != 50 || ExpoMPP != 50 || ExpoStatusRes != 25)
    {
        return false;
    }
    return true;
}
} // namespace

auto runTrustStats1611SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "trust_stats_1611 self-tests failed\n";
        return false;
    }
    return true;
}
