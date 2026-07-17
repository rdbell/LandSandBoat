#pragma once

#include "common/cbasetypes.h"

// Pure CMeritPoints::RaiseMerit / LowerMerit admission plans,
// AddLimitPoints conversion plan, and IsMeritExist bounds gate.
// Spell unlocks, weaponskill unlocks, next-cost table lookup, count mutation,
// and BuildingCharTraitsTable stay host-side after apply.

namespace meritshelpers
{

// ShouldRaiseMerit mirrors the RaiseMerit admission gate:
//   meritPoints >= nextCost && count < upgradeMax && categoryCount < categoryMaxPoints
inline auto ShouldRaiseMerit(
    const uint16 meritPoints,
    const uint16 nextCost,
    const uint8  count,
    const uint8  upgradeMax,
    const uint16 categoryCount,
    const uint8  categoryMaxPoints) -> bool
{
    return meritPoints >= nextCost && count < upgradeMax && categoryCount < categoryMaxPoints;
}

// RaiseMeritPlan is the pure admission/spend outcome for RaiseMerit.
// When apply is true, host subtracts spend from m_MeritPoints, then updates
// next from upgrade tables, runs spell/WS unlock hosts, increments count,
// and rebuilds traits.
struct RaiseMeritPlan
{
    bool   apply{};
    uint16 spend{};
};

// PlanRaiseMerit builds the admission plan. spend is nextCost when apply is
// true and 0 otherwise. Hosts inject m_MeritPoints, PMerit->next/count/upgrade,
// GetMeritCountInSameCategory, and meritCatInfo[cat].MaxPoints.
inline auto PlanRaiseMerit(
    const uint16 meritPoints,
    const uint16 nextCost,
    const uint8  count,
    const uint8  upgradeMax,
    const uint16 categoryCount,
    const uint8  categoryMaxPoints) -> RaiseMeritPlan
{
    if (!ShouldRaiseMerit(meritPoints, nextCost, count, upgradeMax, categoryCount, categoryMaxPoints))
    {
        return {};
    }

    return RaiseMeritPlan{
        .apply = true,
        .spend = nextCost,
    };
}

// ShouldLowerMerit mirrors the LowerMerit count-decrement gate:
//   count > 0
inline auto ShouldLowerMerit(const uint8 count) -> bool
{
    return count > 0;
}

// LowerMeritPlan is the pure admission outcome for LowerMerit.
// When apply is true, host decrements count and refreshes next from the
// category UpgradeID row; spell/WS del hosts stay after apply.
struct LowerMeritPlan
{
    bool apply{};
};

// PlanLowerMerit builds the admission plan. apply is true only when the merit
// pointer is present and count > 0. Hosts inject (PMerit != nullptr) and
// PMerit->count (0 when absent).
inline auto PlanLowerMerit(const bool meritPresent, const uint8 count) -> LowerMeritPlan
{
    if (!meritPresent || !ShouldLowerMerit(count))
    {
        return {};
    }

    return LowerMeritPlan{
        .apply = true,
    };
}

// AddLimitPointsPlan is the pure limit→merit conversion outcome for
// CMeritPoints::AddLimitPoints (slice 2811). Host assigns newLimit/newMerit
// and returns meritIncreased.
struct AddLimitPointsPlan
{
    uint16 newLimit{};
    uint8  newMerit{};
    bool   meritIncreased{};
};

// PlanAddLimitPoints mirrors AddLimitPoints conversion math with injected
// scalars (no settings/GetMeritValue/MAX_LIMIT_POINTS macros):
//   limit = currentLimit + addPoints   // uint16 wrap
//   if limit < maxLimitPoints → no conversion
//   if currentMerit == maxMeritCap → freeze limit at maxLimitPoints-1
//   else newMerit = min(currentMerit + limit/maxLimitPoints, maxMeritCap),
//        newLimit = limit % maxLimitPoints,
//        meritIncreased when newMerit != currentMerit
// Host injects maxMeritCap = map.MAX_MERIT_POINTS + GetMeritValue(MERIT_MAX_MERIT)
// and maxLimitPoints = MAX_LIMIT_POINTS (typically 10000).
inline auto PlanAddLimitPoints(
    const uint16 currentLimit,
    const uint8  currentMerit,
    const uint16 addPoints,
    const uint8  maxMeritCap,
    const uint16 maxLimitPoints) -> AddLimitPointsPlan
{
    // Match LSB uint16 addition (wrap on overflow).
    const uint16 limit = static_cast<uint16>(currentLimit + addPoints);

    if (limit < maxLimitPoints)
    {
        return AddLimitPointsPlan{
            .newLimit       = limit,
            .newMerit       = currentMerit,
            .meritIncreased = false,
        };
    }

    // Cap freeze: hold limit just below the conversion threshold.
    if (currentMerit == maxMeritCap)
    {
        return AddLimitPointsPlan{
            .newLimit       = static_cast<uint16>(maxLimitPoints - 1),
            .newMerit       = currentMerit,
            .meritIncreased = false,
        };
    }

    const uint16 gained = limit / maxLimitPoints;
    uint16       next   = static_cast<uint16>(currentMerit) + gained;
    if (next > static_cast<uint16>(maxMeritCap))
    {
        next = static_cast<uint16>(maxMeritCap);
    }

    const uint8  newMerit = static_cast<uint8>(next);
    const uint16 newLimit = limit % maxLimitPoints;

    if (currentMerit != newMerit)
    {
        return AddLimitPointsPlan{
            .newLimit       = newLimit,
            .newMerit       = newMerit,
            .meritIncreased = true,
        };
    }

    return AddLimitPointsPlan{
        .newLimit       = newLimit,
        .newMerit       = currentMerit,
        .meritIncreased = false,
    };
}

// IsMeritExist mirrors CMeritPoints::IsMeritExist pure bounds gate (slice 2816):
//   merit < categoryStart          → false
//   merit >= categoryCount         → false
//   meritID >= meritsInCat         → false
//   else true
// Hosts inject categoryStart=MCATEGORY_START, categoryCount=MCATEGORY_COUNT,
// meritID=GetMeritID(merit), and meritsInCat from meritCatInfo[cat].MeritsInCat
// (only after the merit is known to be inside the category range so the table
// index is valid; out-of-range injects 0).
inline auto IsMeritExist(
    const int16 merit,
    const int16 categoryStart,
    const int16 categoryCount,
    const uint8 meritID,
    const uint8 meritsInCat) -> bool
{
    if (merit < categoryStart)
    {
        return false;
    }
    if (merit >= categoryCount)
    {
        return false;
    }
    if (meritID >= meritsInCat)
    {
        return false;
    }
    return true;
}

} // namespace meritshelpers
