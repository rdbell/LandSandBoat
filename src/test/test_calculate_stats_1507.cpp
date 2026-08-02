#include "test_calculate_stats_1507.h"

#include "map/calculate_stats_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using calculatestatshelpers::BonusHPStat;
using calculatestatshelpers::ComposeHPScale;
using calculatestatshelpers::ComposeMPScale;
using calculatestatshelpers::ComposeStatScale;
using calculatestatshelpers::ComposeSubJobHP;
using calculatestatshelpers::ComposeSubJobMP;
using calculatestatshelpers::ComposeSubJobStat;
using calculatestatshelpers::FinalBaseStat;
using calculatestatshelpers::FinalMaxHP;
using calculatestatshelpers::FinalMaxMP;
using calculatestatshelpers::MainLevelOver10;
using calculatestatshelpers::MainLevelOver30;
using calculatestatshelpers::MainLevelOver50AndUnder60;
using calculatestatshelpers::MainLevelOver60;
using calculatestatshelpers::MainLevelOver60To75;
using calculatestatshelpers::MainLevelOver75;
using calculatestatshelpers::MainLevelUpTo60;
using calculatestatshelpers::NormalizeMonJob;
using calculatestatshelpers::RaceGroupFromCharRace;
using calculatestatshelpers::ShouldUseSubLevelForRaceMP;
using calculatestatshelpers::SubLevelOver10;
using calculatestatshelpers::SubLevelOver30;

auto nearly(const float a, const float b) -> bool
{
    return std::fabs(a - b) < 1e-4f;
}

auto Check() -> bool
{
    {
        const auto [m, s] = NormalizeMonJob(23, 5);
        if (m != 1 || s != 1)
        {
            return false;
        }
        const auto [m2, s2] = NormalizeMonJob(2, 3);
        if (m2 != 2 || s2 != 3)
        {
            return false;
        }
    }
    if (RaceGroupFromCharRace(1) != 0 || RaceGroupFromCharRace(2) != 0 || RaceGroupFromCharRace(3) != 1 ||
        RaceGroupFromCharRace(7) != 3 || RaceGroupFromCharRace(8) != 4 || RaceGroupFromCharRace(0) != 0)
    {
        return false;
    }
    if (MainLevelOver30(40) != 10 || MainLevelOver30(20) != 0 || MainLevelOver30(70) != 30)
    {
        return false;
    }
    if (MainLevelUpTo60(50) != 49 || MainLevelUpTo60(75) != 59 || MainLevelUpTo60(1) != 0)
    {
        return false;
    }
    if (MainLevelOver60To75(70) != 10 || MainLevelOver60To75(80) != 15 || MainLevelOver75(80) != 5)
    {
        return false;
    }
    if (MainLevelOver10(5) != 0 || MainLevelOver10(20) != 10 || MainLevelOver50AndUnder60(55) != 5)
    {
        return false;
    }
    if (MainLevelOver60(50) != 0 || MainLevelOver60(70) != 10)
    {
        return false;
    }
    if (SubLevelOver10(25) != 15 || SubLevelOver10(5) != 0 || SubLevelOver30(40) != 10 || SubLevelOver30(20) != 0)
    {
        return false;
    }
    if (!nearly(ComposeHPScale(10, 1, 0.5f, 0.2f, 0.1f, 10, 5, 2, 1), 10 + 10 + 2.5f + 0.4f + 0.1f))
    {
        return false;
    }
    if (BonusHPStat(10, 5) != 30)
    {
        return false;
    }
    // base + to60*(slvl-1) + over30*subOver30 + subOver30 + subOver10 all /2
    // 10 + 1*14 + 0.5*5 + 5 + 10 = 10+14+2.5+5+10 = 41.5 / 2 = 20.75
    if (!nearly(ComposeSubJobHP(10, 1, 0.5f, 15, 5, 10), 20.75f))
    {
        return false;
    }
    // uint8_t promotes before subtraction, so zero still contributes -1.
    if (!nearly(ComposeSubJobHP(10, 1, 0.5f, 0, 0, 0), 4.5f))
    {
        return false;
    }
    if (FinalMaxHP(100, 200, 30, 50, 10) != 390)
    {
        return false;
    }
    if (!nearly(ComposeMPScale(5, 1, 0.5f, 10, 4), 5 + 10 + 2))
    {
        return false;
    }
    if (!nearly(ComposeSubJobMP(10, 2, 11, 2.0f), (10 + 20) / 2.0f))
    {
        return false;
    }
    if (!nearly(ComposeSubJobMP(10, 2, 0, 2.0f), 4.0f))
    {
        return false;
    }
    if (!ShouldUseSubLevelForRaceMP(0, 3, 10) || ShouldUseSubLevelForRaceMP(1, 3, 10) || ShouldUseSubLevelForRaceMP(0, 0, 10))
    {
        return false;
    }
    if (FinalMaxMP(10, 20, 5, 3) != 38)
    {
        return false;
    }
    // base + to60*upTo60 + over60*over60 + over75*over75 - 0.01 at mlvl>=75
    if (!nearly(ComposeStatScale(5, 1, 0.5f, 0.2f, 10, 5, 3, 78), 5 + 10 + 2.5f + 0.6f - 0.01f))
    {
        return false;
    }
    if (!nearly(ComposeStatScale(5, 1, 0.5f, 0.2f, 10, 0, 0, 50), 15))
    {
        return false;
    }
    if (!nearly(ComposeSubJobStat(10, 2, 11), (10 + 20) / 2.0f))
    {
        return false;
    }
    if (!nearly(ComposeSubJobStat(10, 2, 0), 4.0f))
    {
        return false;
    }
    if (FinalBaseStat(10, 20, 5, 3) != 38)
    {
        return false;
    }
    return true;
}
} // namespace

auto runCalculateStats1507SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "calculate stats 1507 self-test failed\n";
    }
    return ok;
}
