#pragma once

#include "common/cbasetypes.h"

// Pure CJobPoints helpers dual-wired from job_points.cpp / host call sites:
// - RaiseJobPoint admission/spend plan (slice 2803)
// - IsJobPointExist pure bounds (slice 2815)
// - JobPointCost display/query dual-wire (slice 2828)
// - ShouldApplyRaiseJobPoint spend gate dual-wire (slice 3012)
// SQL UPDATE and jobpointutils::RefreshGiftMods stay host-side.
//
// JobPointCost may already be a host macro from job_points.h; clear it while
// defining the pure helper so the shared name stays testable, then restore.
// Call sites that still have the host macro in scope must use GetJobPointCost
// (macro-safe alias) rather than JobPointCost, which would expand the macro.

#ifdef JobPointCost
#pragma push_macro("JobPointCost")
#undef JobPointCost
#define JOBPOINTSHELPERS_RESTORE_JOBPOINTCOST_MACRO 1
#endif

namespace jobpointshelpers
{

// JobPointCost is the pure form of the JobPointCost macro: (value + 1) % 21.
// Cost is 0 when value is 20, which blocks further raises (cap at 20).
// Prefer GetJobPointCost at production sites that include job_points.h (macro).
inline auto JobPointCost(const uint8 value) -> uint8
{
    return static_cast<uint8>((value + 1) % 21);
}

// GetJobPointCost is the dual-wire-friendly alias of JobPointCost for call
// sites that still have the host JobPointCost macro in scope (function-like
// macros expand even under namespace qualification).
inline auto GetJobPointCost(const uint8 value) -> uint8
{
    return JobPointCost(value);
}

// RaiseJobPointPlan is the pure admission/spend outcome for RaiseJobPoint.
// When apply is true, host subtracts cost from currentJp, adds cost to
// totalJpSpent, increments jobPoint->value, then runs SQL + gift refresh.
struct RaiseJobPointPlan
{
    bool  apply{};
    uint8 cost{};
};

// --- Slice 3012: ShouldApplyRaiseJobPoint pure dual-wire ---
// Residual pure port: slice 2803 (PlanRaiseJobPoint admission/spend plan suite).
// Production host: CJobPoints::RaiseJobPoint injects cost = JobPointCost(value)
// and currentJp into PlanRaiseJobPoint, which dual-wires apply through
// ShouldApplyRaiseJobPoint (job_points.cpp). Display/query path dual-wires the
// same predicate via ShouldRaiseAffordable after JobPointCost(currentValue).
// Go dual-wire: jobpoints.ShouldApplyRaiseJobPoint
// (internal/jobpoints/apply_raise.go).
// Sibling residual: PlanRaiseJobPoint / RaiseJobPointPlan / Cost (2803 suite);
// ShouldRaiseAffordable / GetJobPointCost (2828 suite).

// ShouldApplyRaiseJobPoint mirrors the RaiseJobPoint spend gate half after
// cost is computed.
//
// Formula (slice 3012 dual-wire):
//   cost != 0 && currentJp >= cost
//
// cost      — host-injected JobPointCost(currentValue)
// currentJp — host-injected unspent job points for the category
// true  → host may subtract cost, increment type value, SQL + gift refresh
// false → raise blocked (cap at value 20 when cost==0, or insufficient JP)
//
// Dual-wire of Go jobpoints.ShouldApplyRaiseJobPoint.
// Call sites: PlanRaiseJobPoint (2803), ShouldRaiseAffordable (2828).
// Residual pure port: slice 2803 (raise plan suite).
inline auto ShouldApplyRaiseJobPoint(const uint8 cost, const uint16 currentJp) -> bool
{
    return cost != 0 && currentJp >= cost;
}

// ShouldRaiseAffordable is the display/query form of the spend gate:
// cost = JobPointCost(currentValue); return cost != 0 && currentJp >= cost.
// Does not check type presence (unlike PlanRaiseJobPoint).
inline auto ShouldRaiseAffordable(const uint8 currentValue, const uint16 currentJp) -> bool
{
    return ShouldApplyRaiseJobPoint(JobPointCost(currentValue), currentJp);
}

// PlanRaiseJobPoint short-circuits in production RaiseJobPoint order:
// 1) !jobPresent || !jobPointPresent -> apply=false, cost=0
// 2) cost = JobPointCost(currentValue)
// 3) apply = cost != 0 && currentJp >= cost
// Hosts inject presence of GetJobPointsByType / GetJobPointType results and
// the current type value / unspent JP; mutation stays host-owned.
inline auto PlanRaiseJobPoint(
    const bool   jobPresent,
    const bool   jobPointPresent,
    const uint8  currentValue,
    const uint16 currentJp) -> RaiseJobPointPlan
{
    if (!jobPresent || !jobPointPresent)
    {
        return {};
    }

    const uint8 cost = JobPointCost(currentValue);
    return RaiseJobPointPlan{
        .apply = ShouldApplyRaiseJobPoint(cost, currentJp),
        .cost  = cost,
    };
}

// Pure constants matching JOBPOINTS_* macros from job_points.h (slice 2815).
// Kept local so this header stays free of job_points.h / macro coupling.
inline constexpr uint16 kCategoryCount     = 22;
inline constexpr uint16 kCategoryStart     = 0x020;
inline constexpr uint16 kJPTypePerCategory = 10;
// kMaxTypeLevel matches Cost(20)==0 raise cap / Lua MaxTypeLevel (slice 2828).
inline constexpr uint8 kMaxTypeLevel = 20;

// TotalCostToLevel is the pure sum of raise costs from level 0 to level:
//   sum_{i=0}^{level-1} JobPointCost(i) = level*(level+1)/2 for level <= 20.
// Levels above kMaxTypeLevel clamp to kMaxTypeLevel (210 JP). Level 0 → 0.
inline auto TotalCostToLevel(uint8 level) -> uint16
{
    if (level > kMaxTypeLevel)
    {
        level = kMaxTypeLevel;
    }
    return static_cast<uint16>(static_cast<uint16>(level) * static_cast<uint16>(level + 1) / 2);
}

// CategoryIndexByType mirrors JobPointsCategoryIndexByJpType: jpType >> 5.
// Named to avoid clashing with the host macro of the same shape.
inline auto CategoryIndexByType(const uint16 jpType) -> uint16
{
    return static_cast<uint16>(jpType >> 5);
}

// TypeIndex mirrors JobPointTypeIndex: id & 0x1F.
// Named to avoid clashing with the host JobPointTypeIndex macro.
inline auto TypeIndex(const uint16 id) -> uint8
{
    return static_cast<uint8>(id & 0x1F);
}

// IsJobPointExistPure mirrors CJobPoints::IsJobPointExist pure bounds checks:
// 1) jpType < JOBPOINTS_CATEGORY_START
// 2) (JobPointsCategoryIndexByJpType(jpType) - 1 > JOBPOINTS_CATEGORY_COUNT)
// 3) (JobPointTypeIndex(jpType) > JOBPOINTS_JPTYPE_PER_CATEGORY)
//
// Quirks preserved for parity:
// - category index 23 is accepted ((23-1) > 22 is false); 24 is rejected
// - type index == JPTypePerCategory (10) is accepted (check is >); 11 rejected
inline auto IsJobPointExistPure(const uint16 jpType) -> bool
{
    if (jpType < kCategoryStart)
    {
        return false;
    }
    if (CategoryIndexByType(jpType) - 1 > kCategoryCount)
    {
        return false;
    }
    if (TypeIndex(jpType) > kJPTypePerCategory)
    {
        return false;
    }
    return true;
}

} // namespace jobpointshelpers

#ifdef JOBPOINTSHELPERS_RESTORE_JOBPOINTCOST_MACRO
#pragma pop_macro("JobPointCost")
#undef JOBPOINTSHELPERS_RESTORE_JOBPOINTCOST_MACRO
#endif
