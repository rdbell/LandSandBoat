#pragma once

#include "common/cbasetypes.h"

// Pure Treasure Casket drop-chance helpers shared by dual-wire slices:
//   - 0917: residual pure port (internal/casket DropThreshold / DropChance)
//   - 2878: DropThreshold / DropChance residual dual-wire suite (drop_chance)
//   - 3155: DropThreshold dedicated dual-wire (drop_chance.go / drop_threshold_test.go)
//
// Dual-wire index:
//   - 2878: DropThreshold / DropChance residual dual-wire suite
//   - 3155: DropThreshold = clamp(base + kupo + prowess, 0, 1)
//
// Lua production host: scripts/globals/caskets.lua local function dropChance:
//
//   local rand = math.random()
//   if rand < utils.clamp(CASKET_DROP_RATE + kupowersMMBPower
//                         + prowessCasketsPower, 0, 1) then
//     return true
//   end
//   return false
//
// Host injects scalars only (no player / entity pointers):
//   baseRate            — xi.settings.main.CASKET_DROP_RATE (often 0.1)
//   kupowersMMBPower    — Super Kupowers Myriad Mystery Boxes power (0 if absent)
//   prowessCasketsPower — PROWESS_CASKET_RATE power / 100 (0 if absent)
//   roll                — math.random() result in [0, 1)
//
// Status-effect lookup, settings map binding, RNG, and casket spawn remain
// host-owned.
// Go dual-wire: casket.DropThreshold (internal/casket/drop_chance.go).
// Residual dual-wire suite: 2878 (test_casket_drop_chance_2878).
// Dedicated dual-wire suite: 3155 (test_casket_drop_threshold_3155).
// Prior pure port: OmegaXI slice 0917 (internal/casket).

namespace caskethelpers
{

// Clamp mirrors utils.clamp(v, lo, hi) for the drop-rate product.
// Residual helper shared by DropThreshold (3155 / 2878 / 0917).
inline auto Clamp(const double v, const double lo, const double hi) -> double
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

// DropThreshold is the pure clamped discovery rate from dropChance once base
// rate and status powers are injected:
//
//   utils.clamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0, 1)
//
// Formula (slice 3155 dedicated dual-wire; residual expand 2878 / pure 0917 —
// formula unchanged):
//   DropThreshold(baseRate, kupowersMMBPower, prowessCasketsPower)
//     = Clamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0, 1)
//
// baseRate            — xi.settings.main.CASKET_DROP_RATE (often 0.1)
// kupowersMMBPower    — Super Kupowers MMB power (0 if absent / unimplemented)
// prowessCasketsPower — PROWESS_CASKET_RATE power / 100 (0 if absent)
//
// Dual-wire of Go casket.DropThreshold.
// Call site: future Lua dropChance inject (threshold half).
// Prior pure port: slice 0917. Residual dual-wire suite: 2878 /
// test_casket_drop_chance_2878. Dedicated dual-wire suite is
// test_casket_drop_threshold_3155. Host still owns status-effect lookup,
// settings map binding, math.random, and casket spawn writeback.
// Future Lua host injects scalars into this helper instead of re-inlining
// the clamp sum.
inline auto DropThreshold(const double baseRate, const double kupowersMMBPower, const double prowessCasketsPower) -> double
{
    return Clamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0.0, 1.0);
}

// DropChance reports whether a precomputed uniform roll succeeds against the
// clamped drop threshold:
//
//   roll < DropThreshold(...)
//
// Success is strict less-than (not ≤), matching Lua.
// Sibling residual only under slice 3155 (dedicated DropThreshold expand);
// dual-wire surface remains residual 2878 / pure 0917. Formula unchanged.
// Dual-wire of Go casket.DropChance.
inline auto DropChance(const double baseRate, const double kupowersMMBPower, const double prowessCasketsPower, const double roll) -> bool
{
    return roll < DropThreshold(baseRate, kupowersMMBPower, prowessCasketsPower);
}

} // namespace caskethelpers
