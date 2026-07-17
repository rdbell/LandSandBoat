#pragma once

#include "common/cbasetypes.h"

// Pure Apkallu helpers for dual-wire slices:
//   - 2880: CanRunAway (getHateTier(hate) >= 3)
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
// Dual-wire of Go apkallu.CanRunAway / GetHateTier (slice 2880).

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
