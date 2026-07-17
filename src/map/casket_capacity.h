#pragma once

#include "common/cbasetypes.h"

// Pure Treasure Casket drop-chance helpers shared by dual-wire slices:
//   - 2878: DropThreshold / DropChance (caskets.lua dropChance inject form)
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
// host-owned. Prior pure port: OmegaXI slice 0917 (internal/casket).

namespace caskethelpers
{

// Clamp mirrors utils.clamp(v, lo, hi) for the drop-rate product.
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
inline auto DropChance(const double baseRate, const double kupowersMMBPower, const double prowessCasketsPower, const double roll) -> bool
{
    return roll < DropThreshold(baseRate, kupowersMMBPower, prowessCasketsPower);
}

} // namespace caskethelpers
