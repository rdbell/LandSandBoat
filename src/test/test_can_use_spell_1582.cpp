#include "test_can_use_spell_1582.h"

#include "map/can_use_spell_capacity.h"

#include <iostream>

namespace
{
using namespace canusespellhelpers;

auto Check() -> bool
{
    if (GiftCatalogSize() != 60)
    {
        return false;
    }
    // Known gifts.
    if (GiftCost(JobWHM, 848) != GiftJP100 || GiftCost(JobWHM, 893) != GiftJP1200)
    {
        return false;
    }
    if (GiftCost(JobBLM, 849) != GiftJP100 || GiftCost(JobBLM, 881) != GiftJP550 || GiftCost(JobBLM, 367) != GiftJP1200)
    {
        return false;
    }
    if (GiftCost(JobRDM, 148) != GiftJP100 || GiftCost(JobRDM, 895) != GiftJP1200)
    {
        return false;
    }
    if (GiftCost(JobPLD, 855) != GiftJP100 || GiftCost(JobDRK, 880) != GiftJP100)
    {
        return false;
    }
    if (GiftCost(JobNIN, 340) != GiftJP100 || GiftCost(JobRUN, 493) != GiftJP550)
    {
        return false;
    }
    if (GiftCost(JobSCH, 888) != GiftJP1200 || GiftCost(JobGEO, 865) != GiftJP1200)
    {
        return false;
    }
    // Missing.
    if (GiftCost(1 /* WAR */, 148) != UnknownCost || GiftCost(JobWHM, 367) != UnknownCost)
    {
        return false;
    }
    if (GiftCost(JobBLM, 148 /* Fire V */) != UnknownCost || GiftCost(JobRDM, 493 /* Temper */) != UnknownCost)
    {
        return false;
    }
    // Thresholds.
    if (CanUseOverride(JobWHM, 848, 99) || !CanUseOverride(JobWHM, 848, 100) || !CanUseOverride(JobWHM, 848, 500))
    {
        return false;
    }
    if (CanUseOverride(JobBLM, 367, 1199) || !CanUseOverride(JobBLM, 367, 1200))
    {
        return false;
    }
    if (CanUseOverride(JobRUN, 493, 549) || !CanUseOverride(JobRUN, 493, 550))
    {
        return false;
    }
    if (CanUseOverride(JobWHM, 367, 9999))
    {
        return false;
    }
    // Spent JP host gate.
    if (SpentJobPointsForOverride(false, 99, 500) != 0)
    {
        return false;
    }
    if (SpentJobPointsForOverride(true, 98, 500) != 0)
    {
        return false;
    }
    if (SpentJobPointsForOverride(true, 99, 500) != 500)
    {
        return false;
    }
    return true;
}
} // namespace

auto runCanUseSpell1582SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "can_use_spell_1582 self-tests failed\n";
        return false;
    }
    return true;
}
