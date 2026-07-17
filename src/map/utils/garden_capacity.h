#pragma once

#include "common/cbasetypes.h"

// Pure gardenutils helpers dual-wired from gardenutils.cpp:
// - detail::AppendResult / LoadResultList row insert (slice 2838)
// - CalculateResults resultUid lookup (slice 2838)
// - UpdateGardening wilt branch (slice 3339 dedicated dual-wire expand
//   residual 2926; residual pure inject 0795; residual dual-wire 2926)
//
// Helpers take host-injected seed + crystal-feed element IDs / wilt scalars
// only (no DB, flowerpot, or map storage).

namespace gardenutilshelpers
{

// ResultKey packs plant seed + common/extra crystal feeds into the
// gardening_results map key used by LoadResultList and CalculateResults:
//
//   uid = (seed << 8) + (element1 << 4) + element2
//
// seed is FLOWERPOT_PLANT_TYPE; element1/element2 are FLOWERPOT_ELEMENT_TYPE.
inline auto ResultKey(const uint8 seed, const uint8 element1, const uint8 element2) -> uint32
{
    return (seed << 8) + (element1 << 4) + element2;
}

// Wilt timing constants from gardenutils.cpp (UpdateGardening) / Go wilt.go.
// Pinned by residual 2926 / dedicated 3339 dual-wire tests.
inline constexpr uint32 VanaDaysToWilt           = 36;
inline constexpr uint32 VanaDaysToGuaranteeWilt = 144;

// ShouldWilt is the pure wilt branch of UpdateGardening once stage duration
// (vanadays from GetStageDuration), days since stage change, wilt bonus
// (Mod::GARDENING_WILT_BONUS), and examined flag are known
// (slice 3339 dedicated dual-wire expand residual 2926; residual pure 0795;
// residual dual-wire suite 2926 — formula UNCHANGED / bit-identical):
//
//   wiltTime = VANADAYS_TO_WILT + wiltBonus
//   wilt if:
//     daysSince > VANADAYS_TO_GUARANTEE_WILT + wiltTime
//     else if wasExamined: false
//     else stageDuration > wiltTime || stageDuration + daysSince > wiltTime
//
// Positive early-return form only (match Go free function / pinShouldWilt3339;
// avoid QF1001 De Morgan rewrites of the production compound OR).
//
// Host injects stageDurationVanadays, daysSinceStageChange, wiltBonus, and
// wasExamined only (no CCharEntity* / CItemFlowerpot*).
// Production site: UpdateGardening wilt if in gardenutils.cpp.
// Coverage: test_garden_should_wilt_3339 (dedicated); residual 2926 retained.
inline auto ShouldWilt(const uint32 stageDurationVanadays, const uint32 daysSinceStageChange,
                       const uint32 wiltBonus, const bool wasExamined) -> bool
{
    const uint32 wiltTime = VanaDaysToWilt + wiltBonus;
    if (daysSinceStageChange > VanaDaysToGuaranteeWilt + wiltTime)
    {
        return true;
    }
    if (wasExamined)
    {
        return false;
    }
    return stageDurationVanadays > wiltTime ||
           stageDurationVanadays + daysSinceStageChange > wiltTime;
}

} // namespace gardenutilshelpers
