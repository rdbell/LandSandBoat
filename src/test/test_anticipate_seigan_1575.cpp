#include "test_anticipate_seigan_1575.h"

#include "map/physical_hit_rate_capacity.h"
#include "map/seigan_counter_capacity.h"

#include <iostream>

namespace
{
using namespace physicalhitratehelpers;
using namespace seigancounterhelpers;

auto Check() -> bool
{
    // CanRetain: non-PC always; PC needs 2H
    if (!CanRetainThirdEye(false, false) || !CanRetainThirdEye(false, true) ||
        CanRetainThirdEye(true, false) || !CanRetainThirdEye(true, true))
    {
        return false;
    }

    // Retention: t=0, mod=0 → 100*100 = 10000
    if (ThirdEyeRetentionScaledChance(0, 0) != 10000)
    {
        return false;
    }
    // Full 30s elapsed, no mod → 0
    if (ThirdEyeRetentionScaledChance(30000, 0) != 0)
    {
        return false;
    }
    // Retains / deletes
    if (!RetainsThirdEye(5000, 5000) || RetainsThirdEye(5000, 5001) || RetainsThirdEye(0, 1))
    {
        return false;
    }

    // Anticipate retention path: no seigan → always delete
    {
        const auto r = CheckAnticipatedRetention(false, true, 0, 0, 1);
        if (!r.anticipated || !r.shouldDeleteThirdEye)
        {
            return false;
        }
    }
    // Seigan + retain + full chance + low roll → keep
    {
        const auto r = CheckAnticipatedRetention(true, true, 0, 0, 1);
        if (!r.anticipated || r.shouldDeleteThirdEye)
        {
            return false;
        }
    }

    // Seigan counter gates
    if (CheckSeiganCounter(false, true, true, true, false, false, 0, 1.0, 1))
    {
        return false;
    }
    if (CheckSeiganCounter(true, false, true, true, false, false, 0, 1.0, 1))
    {
        return false;
    }
    if (CheckSeiganCounter(true, true, false, true, false, false, 0, 1.0, 1))
    {
        return false;
    }
    if (CheckSeiganCounter(true, true, true, false, false, false, 0, 1.0, 1))
    {
        return false;
    }
    // PC without 2H
    if (CheckSeiganCounter(true, true, true, true, true, false, 0, 1.0, 1))
    {
        return false;
    }
    // Happy path: base 25, hit 1.0, roll 25 → true; roll 26 → false
    if (!CheckSeiganCounter(true, true, true, true, false, false, 0, 1.0, 25) ||
        CheckSeiganCounter(true, true, true, true, false, false, 0, 1.0, 26))
    {
        return false;
    }
    // hitRateFactor 0.5 → threshold 12.5; roll 12 true, 13 false
    if (!CheckSeiganCounter(true, true, true, true, false, false, 0, 0.5, 12) ||
        CheckSeiganCounter(true, true, true, true, false, false, 0, 0.5, 13))
    {
        return false;
    }

    return true;
}
} // namespace

auto runAnticipateSeigan1575SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "anticipate_seigan_1575 self-tests failed\n";
        return false;
    }
    return true;
}
