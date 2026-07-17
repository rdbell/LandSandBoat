#pragma once

#include "common/cbasetypes.h"

// Pure CMeritPoints::RaiseMerit / LowerMerit admission plans,
// AddLimitPoints conversion plan, and IsMeritExist bounds gate.
// Spell unlocks, weaponskill unlocks, next-cost table lookup, count mutation,
// and BuildingCharTraitsTable stay host-side after apply.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2805: PlanRaiseMerit (RaiseMerit admission plan; uses ShouldRaiseMerit)
//   - 3160: ShouldRaiseMerit (points/upgrade/category RaiseMerit admission gate half)
//   - 2810: PlanLowerMerit (LowerMerit admission plan; uses ShouldLowerMerit)
//   - 2811: PlanAddLimitPoints (AddLimitPoints conversion plan)
//   - 2816: IsMeritExist (IsMeritExist bounds gate residual pure port)
//   - 3196: IsMeritExist (range + MeritsInCat bounds gate dual-wire expand of 2816)
//   - 3054: ShouldLowerMerit residual dual-wire (count > 0 LowerMerit count-decrement gate half)
//   - 3256: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3373: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3424: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3478: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3537: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3581: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3626: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3671: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3716: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3761: ShouldLowerMerit dedicated dual-wire expand residual 3054
//
// Production host: CMeritPoints::RaiseMerit (merit.cpp) injects m_MeritPoints,
// PMerit->next/count/upgrade, GetMeritCountInSameCategory, and
// meritCatInfo[cat].MaxPoints into PlanRaiseMerit / ShouldRaiseMerit; on apply
// spends points, refreshes next, optional spell/WS unlock, count++, traits.
// Production host: CMeritPoints::LowerMerit (merit.cpp) injects
// (PMerit != nullptr) and PMerit->count into PlanLowerMerit / ShouldLowerMerit;
// on apply decrements count, refreshes next from upgrade tables, optional
// spell/WS del hosts.
// Production host: CMeritPoints::IsMeritExist (merit.cpp) injects
// MCATEGORY_START/COUNT, GetMeritID(merit), and meritCatInfo[cat].MeritsInCat
// into IsMeritExist; pure range + MeritsInCat only (no GMeritsTemplate).
// Go dual-wire: merit.ShouldRaiseMerit (internal/merit/raise_merit.go);
// merit.PlanRaiseMerit (internal/merit/entry.go residual 2805);
// merit.ShouldLowerMerit (internal/merit/lower_merit.go);
// merit.PlanLowerMerit (internal/merit/entry.go residual 2810);
// merit.IsMeritExistBounds / merit.IsMeritExist (internal/merit/catinfo.go).

namespace meritshelpers
{

// ShouldRaiseMerit mirrors the RaiseMerit admission gate.
//
// Formula (slice 3160 dual-wire):
//   meritPoints >= nextCost && count < upgradeMax && categoryCount < categoryMaxPoints
//
// meritPoints       — host-evaluated m_MeritPoints
// nextCost          — host-evaluated PMerit->next (upgrade table stays host-side)
// count             — host-evaluated Merit_t.count
// upgradeMax        — host-evaluated Merit_t.upgrade
// categoryCount     — host GetMeritCountInSameCategory total
// categoryMaxPoints — host meritCatInfo[cat].MaxPoints
// true  → admit RaiseMerit spend + count++ path (host subtracts nextCost,
//         refreshes next, optional spell/WS unlock, BuildingCharTraitsTable)
// false → block raise (insufficient points, at upgrade max, or category full)
//
// Isolated three-way admission gate half of PlanRaiseMerit / CMeritPoints::RaiseMerit.
// Merit pointer presence is a separate RaiseMerit host check (null → return);
// this free function only checks points, upgrade max, and category cap.
//
// Dual-wire of Go merit.ShouldRaiseMerit.
// Call site: CMeritPoints::RaiseMerit via PlanRaiseMerit — host injects balances,
// entry fields, category totals, and caps; on true host spends and mutates.
// Prior pure port: slice 2805 (RaiseMerit admission plan suite). Residual pins
// remain in test_merit_raise_plan_2805; dedicated dual-wire suite is
// test_merit_should_raise_merit_3160. Sibling residual: PlanRaiseMerit (2805).
// Sibling dual-wire: ShouldLowerMerit (3761 expand residual 3054) is independent.
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

// --- Slice 3761: ShouldLowerMerit dedicated dual-wire expand residual 3054 ---
// Dual-wire index:
//   - 2810: residual pure port (PlanLowerMerit / ShouldLowerMerit admission suite)
//   - 3054: ShouldLowerMerit residual dual-wire suite
//   - 3256: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3373: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3424: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3478: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3537: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3581: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3626: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3671: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3716: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3761: ShouldLowerMerit dedicated dual-wire expand residual 3054
// Dual-wire pure free functions (formula space):
//   - 2810 / 3054: ShouldLowerMerit residual pure dual-wire
//   - 3256: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3373: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3424: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3478: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3537: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3581: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3626: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3671: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3716: ShouldLowerMerit prior dedicated dual-wire expand residual 3054
//   - 3761: ShouldLowerMerit = count > 0
//     dedicated dual-wire expand residual 3054
//
// Residual pure port: slice 2810 (LowerMerit admission plan suite).
// Residual dual-wire: slice 3054 (ShouldLowerMerit free-function dual-wire suite).
// Prior dedicated dual-wire: slice 3256 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3373 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3424 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3478 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3537 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3581 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3626 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3671 (dedicated expand residual 3054 suite).
// Prior dedicated dual-wire: slice 3716 (dedicated expand residual 3054 suite).
// Production host: CMeritPoints::LowerMerit injects (PMerit != nullptr) and
// PMerit->count into PlanLowerMerit / ShouldLowerMerit; on apply decrements
// count, refreshes next, optional spell/WS del hosts.
// Go dual-wire: merit.ShouldLowerMerit (internal/merit/lower_merit.go; slice 3761).
// Coverage: test_merit_lower_3054 (residual dual-wire),
// test_merit_lower_merit_3256 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3373 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3424 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3478 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3537 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3581 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3626 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3671 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3716 (prior dedicated expand residual 3054),
// test_merit_lower_merit_3761 (dedicated expand residual 3054; not in
// CMake/main). Residual 3054 / prior dedicated 3716 / 3671 / 3626 / 3581 / 3537 / 3478 / 3424 / 3373 / 3256 suites retained.
//
// Dual-wire notes (slice 3761):
//   Formula unchanged from pure 2810 / residual dual-wire 3054 / prior dedicated
//   3716 / 3671 / 3626 / 3581 / 3537 / 3478 / 3424 / 3373 / 3256:
//     ShouldLowerMerit(count) = count > 0
//   free == inline == pin == pin3716 (direct return). Residual poles (0, 1, n) + dense.
//   Residual 3054 / prior dedicated 3716 / 3671 / 3626 / 3581 / 3537 / 3478 / 3424 / 3373 / 3256 suites retained.
//
// ShouldLowerMerit mirrors the LowerMerit count-decrement gate.
//
// Formula (slice 3761 dedicated dual-wire expand residual 3054; pure 2810 /
// residual 3054 / prior dedicated 3716 / 3671 / 3626 / 3581 / 3537 / 3478 / 3424 / 3373 / 3256 — formula unchanged):
//   count > 0
//
// count — host-evaluated Merit_t.count (0 when merit pointer absent)
// true  → admit LowerMerit count decrement (host refreshes next, may del spell/WS)
// false → block lower (count already zero)
//
// Isolated count-decrement gate half of PlanLowerMerit / CMeritPoints::LowerMerit.
// Points are not refunded. Presence of the merit pointer is a separate
// PlanLowerMerit inject (meritPresent); this free function only checks count > 0.
//
// Dual-wire of Go merit.ShouldLowerMerit (lower_merit.go / slice 3761).
// Call site: CMeritPoints::LowerMerit via PlanLowerMerit — host injects
// PMerit->count (0 when absent); on true host decrements count and refreshes next.
// Coverage: test_merit_lower_merit_3761 (dedicated expand residual 3054;
// not in CMake/main); residual 3054 / prior dedicated 3716 / 3671 / 3626 / 3581 / 3537 / 3478 / 3424 / 3373 / 3256 suites retained.
// Sibling residual: PlanLowerMerit (2810). Sibling dual-wire: ShouldRaiseMerit
// (3160) is independent.
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

// IsMeritExist mirrors CMeritPoints::IsMeritExist pure bounds gate.
//
// Formula (slice 3196 dual-wire; residual pure port 2816):
//   merit < categoryStart          → false
//   merit >= categoryCount         → false
//   meritID >= meritsInCat         → false
//   else true
//
// Compound form (equivalent):
//   merit >= categoryStart && merit < categoryCount && meritID < meritsInCat
//
// merit          — host-evaluated MERIT_TYPE as int16
// categoryStart  — host-evaluated MCATEGORY_START (0x0040)
// categoryCount  — host-evaluated MCATEGORY_COUNT (0x0DC0)
// meritID        — host-evaluated GetMeritID(merit) (((merit) & 0x3F) >> 1)
// meritsInCat    — host-evaluated meritCatInfo[cat].MeritsInCat (0 when out of range)
// true  → merit type is inside category range with a valid id for that category
// false → below start, at/past categoryCount, or meritID overflows MeritsInCat
//
// Isolated three-way bounds gate of CMeritPoints::IsMeritExist. Does not
// consult the full merits[] / GMeritsTemplate table — only range + MeritsInCat.
//
// Dual-wire of Go merit.IsMeritExistBounds.
// Call site: CMeritPoints::IsMeritExist — host injects CategoryStart/Count,
// GetMeritID, and MeritsInCat (category table only when merit is in range;
// else 0). Prior pure port: slice 2816 (IsMeritExist residual suite). Residual
// pins remain in test_merit_exist_2816; dedicated dual-wire suite is
// test_merit_is_merit_exist_3196. Sibling dual-wires left alone:
// ShouldRaiseMerit (3160), ShouldLowerMerit (3761 expand residual 3054).
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
