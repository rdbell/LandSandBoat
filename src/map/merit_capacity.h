#pragma once

#include "common/cbasetypes.h"

// Pure CMeritPoints::RaiseMerit admission plan.
// Spell unlocks, weaponskill unlocks, next-cost table lookup, count++, and
// BuildingCharTraitsTable stay host-side after apply.

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

} // namespace meritshelpers
