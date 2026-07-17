#pragma once

#include "common/cbasetypes.h"

// Pure Apkallu helpers for dual-wire slices:
//   - 2880: CanRunAway residual dual-wire suite (getHateTier(hate) >= 3)
//   - 3149: CanRunAway prior dedicated dual-wire (can_run_away.go)
//   - 3245: CanRunAway prior dedicated dual-wire expand residual 2880
//   - 3287: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3245)
//   - 3317: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3287)
//   - 3407: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3317)
//   - 3460: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3407)
//   - 3505: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3460)
//   - 3558: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3505)
//   - 3603: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3558)
//   - 3648: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3603)
//   - 3693: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3648)
//   - 3738: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3693)
//   - 3783: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3738)
//   - 3828: CanRunAway prior dedicated dual-wire expand residual 2880 (prior ~3783)
//   - 3873: CanRunAway dedicated dual-wire expand residual 2880 (prior ~3828)
//
// Dual-wire index:
//   - 2880: CanRunAway residual dual-wire suite
//   - 3149: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3245: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3287: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3317: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3407: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3460: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3505: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3558: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3603: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3648: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3693: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3738: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3783: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3828: CanRunAway prior dedicated (GetHateTier(hate) >= 3 ≡ hate >= 45)
//   - 3873: CanRunAway (GetHateTier(hate) >= 3 ≡ hate >= 45)
//
// Lua production host: scripts/globals/apkallu.lua
//   xi.apkallu.canRunAway = function(mob)
//     local zoneID = mob:getZoneID()
//     local hate = GetServerVariable('ApkalluHate_'..zoneID)
//     return xi.apkallu.getHateTier(hate) >= 3
//   end
//
// Host injects scalars only (no mob / zone pointers):
//   hate — GetServerVariable('ApkalluHate_'..zoneID)
//
// Entity track / initialize / setLocalVar('RunAway') remains host-owned.
// Prior pure port: OmegaXI slice 0925 (internal/apkallu).
// Residual dual-wire suite: slice 2880 / test_apkallu_can_run_away_2880.
// Prior dedicated dual-wire suite: slice 3149 / test_apkallu_can_run_away_3149.
// Prior dedicated dual-wire suite: slice 3245 / test_apkallu_can_run_away_3245.
// Prior dedicated dual-wire suite: slice 3287 / test_apkallu_can_run_away_3287.
// Prior dedicated dual-wire suite: slice 3317 / test_apkallu_can_run_away_3317.
// Prior dedicated dual-wire suite: slice 3407 / test_apkallu_can_run_away_3407.
// Prior dedicated dual-wire suite: slice 3460 / test_apkallu_can_run_away_3460.
// Prior dedicated dual-wire suite: slice 3505 / test_apkallu_can_run_away_3505.
// Prior dedicated dual-wire suite: slice 3558 / test_apkallu_can_run_away_3558.
// Prior dedicated dual-wire suite: slice 3603 / test_apkallu_can_run_away_3603.
// Prior dedicated dual-wire suite: slice 3648 / test_apkallu_can_run_away_3648.
// Prior dedicated dual-wire suite: slice 3693 / test_apkallu_can_run_away_3693.
// Prior dedicated dual-wire suite: slice 3738 / test_apkallu_can_run_away_3738.
// Prior dedicated dual-wire suite: slice 3783 / test_apkallu_can_run_away_3783.
// Prior dedicated dual-wire suite: slice 3828 / test_apkallu_can_run_away_3828.
// Dedicated dual-wire suite: slice 3873 / test_apkallu_can_run_away_3873.
// Dual-wire of Go apkallu.CanRunAway / GetHateTier.

namespace apkalluhelpers
{

// Hate tier thresholds (inclusive lower bounds), matching
// xi.apkallu.getHateTier / Go HateTier{1,2,3}Min:
//   hate >= 45 → 3, >= 25 → 2, >= 5 → 1, else 0
inline constexpr int32 kHateTier1Min = 5;
inline constexpr int32 kHateTier2Min = 25;
inline constexpr int32 kHateTier3Min = 45;

// GetHateTier maps zone Apkallu hate to tier 0..3.
// Dual-wire of Go apkallu.GetHateTier / xi.apkallu.getHateTier.
inline auto GetHateTier(const int32 hate) -> int32
{
    if (hate >= kHateTier3Min)
    {
        return 3;
    }
    if (hate >= kHateTier2Min)
    {
        return 2;
    }
    if (hate >= kHateTier1Min)
    {
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Slice 2880 / 3149 / 3245 / 3287 / 3317 / 3407 / 3460 / 3505 / 3558 / 3603 / 3648 / 3693 / 3738 / 3783 / 3828 / 3873 — CanRunAway pure dual-wire
//
// Formula (slice 3873 dedicated dual-wire expand residual 2880; prior
// dedicated ~3828 / 3783 / 3738 / 3693 / 3648 / 3603 / 3558 / 3505 / 3460 / 3407 / 3317 / 3287 / 3245 / 3149; pure inject 0925 — formula unchanged):
//   CanRunAway(hate) = GetHateTier(hate) >= 3
//   // ≡ hate >= kHateTier3Min (45)
//
// Dual-wire of Go apkallu.CanRunAway.
// Call site: future Lua canRunAway / SPAWN mixin inject.
// Prior pure port: slice 0925. Residual dual-wire suite: 2880 /
// test_apkallu_can_run_away_2880. Prior dedicated dual-wire suites:
// 3149 / test_apkallu_can_run_away_3149, 3245 /
// test_apkallu_can_run_away_3245, 3287 /
// test_apkallu_can_run_away_3287, 3317 /
// test_apkallu_can_run_away_3317, 3407 /
// test_apkallu_can_run_away_3407, 3460 /
// test_apkallu_can_run_away_3460, 3505 /
// test_apkallu_can_run_away_3505, 3558 /
// test_apkallu_can_run_away_3558, 3603 /
// test_apkallu_can_run_away_3603, 3648 /
// test_apkallu_can_run_away_3648, 3693 /
// test_apkallu_can_run_away_3693, 3738 /
// test_apkallu_can_run_away_3738, 3783 /
// test_apkallu_can_run_away_3783, and 3828 /
// test_apkallu_can_run_away_3828. Dedicated dual-wire suite is
// test_apkallu_can_run_away_3873. Host still owns getZoneID, server-variable
// load/store, and setLocalVar('RunAway') writeback.
// ---------------------------------------------------------------------------

// CanRunAway is the pure half of xi.apkallu.canRunAway once hate is injected:
//   GetHateTier(hate) >= 3
// true when zone hate is at tier 3 (hate >= 45), matching the SPAWN mixin
// that sets local var RunAway = 1.
// Dual-wire of Go apkallu.CanRunAway.
inline auto CanRunAway(const int32 hate) -> bool
{
    return GetHateTier(hate) >= 3;
}

} // namespace apkalluhelpers
