#pragma once

#include "common/cbasetypes.h"

// Pure Besieged standing onEventUpdate option-gate helpers (slice 2859).
//
// Production host is Lua scripts/globals/besieged.lua xi.besieged.onEventUpdate:
//
//   local entry = imperialStandingItems[option]
//   if not entry then return end
//   if option < 0x40000000 then
//     player:updateEvent(...)
//   end
//
// Capacity is for future Lua/C++ inject so hosts dual-wire the pure gate
// instead of re-inlining the comparison. Helpers take host-injected scalars
// only (no entity / currency pointers). updateEvent payload remains host-owned.
//
// Note: namespace is besiegedhelpers (not besieged) to avoid clashing with the
// map besieged_system free getters.

namespace besiegedhelpers
{

// ItemOptionCeiling is the onEventUpdate / onEventFinish item-path gate
// (option < 0x40000000). Same numeric value as cipherValue when active.
constexpr uint32 ItemOptionCeiling = 0x40000000u;

// IsItemOptionPath reports option < ItemOptionCeiling (shared item-path gate).
inline auto IsItemOptionPath(const uint32 option) -> bool
{
    return option < ItemOptionCeiling;
}

// ShouldUpdateStandingEvent is the pure free-function form of the onEventUpdate
// option gate:
//   option < 0x40000000
// Dual-wires IsItemOptionPath. Host looks up the catalog entry first; missing
// rows return early without calling this gate.
inline auto ShouldUpdateStandingEvent(const uint32 option) -> bool
{
    return IsItemOptionPath(option);
}

} // namespace besiegedhelpers
