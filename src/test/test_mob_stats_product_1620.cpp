#include "test_mob_stats_product_1620.h"

#include "map/mob_stats_product_capacity.h"

#include <iostream>

namespace
{
using namespace mobstatsproducthelpers;

auto Check() -> bool
{
    if (ClampSettingsMultiplier(1.5f) != 1.5f || ClampSettingsMultiplier(3.0f) != 1.0f || ClampSettingsMultiplier(0.05f) != 1.0f)
    {
        return false;
    }
    if (CombineMobHP(1000, 200, false) != 1200 || CombineMobHP(1000, 200, true) != 360)
    {
        return false;
    }
    if (ApplyHPMultiplier(1000, 1.5f) != 1500 || ApplyHPMultiplier(1000, 3.0f) != 1000)
    {
        return false;
    }
    if (ApplyMPMultiplier(1000, 1.5f) != 1500 || ApplyMPMultiplier(1000, 0.05f) != 1000)
    {
        return false;
    }
    if (!JobGivesMP(JobPLD) || !JobGivesMP(JobWHM) || !JobGivesMP(JobBLM) || !JobGivesMP(JobRDM) ||
        !JobGivesMP(JobDRK) || !JobGivesMP(JobBLU) || !JobGivesMP(JobSCH) || !JobGivesMP(JobSMN) ||
        JobGivesMP(0) || JobGivesMP(1) || JobGivesMP(255))
    {
        return false;
    }
    if (!JobHasMP(JobWHM, 1) || !JobHasMP(1, JobBLM) || JobHasMP(1, 2))
    {
        return false;
    }
    if (ResolveMPScale(1.2f, 0) != 1.2f || ResolveMPScale(1.2f, 50) != 0.5f)
    {
        return false;
    }
    if (CalculateMobMaxMP(1, 1.0f) != 28)
    {
        return false;
    }
    if (SumStat(10, 20, 5) != 35 || ApplyStatMultiplier(100, 1.5f) != 150 || HalveSubJobStat(11) != 5)
    {
        return false;
    }
    return true;
}
} // namespace

auto runMobStatsProduct1620SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "mob_stats_product_1620 self-tests failed\n";
        return false;
    }
    return true;
}
