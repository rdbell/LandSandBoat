#pragma once

#include "common/cbasetypes.h"

// Pure Besieged free-function dual-wire helpers shared by slices:
//   - 2859: ShouldUpdateStandingEvent / IsItemOptionPath (onEventUpdate gate)
//   - 2945: CanAffordSanction / SanctionAffordCost (onEventFinish sanction
//           imperial_standing afford gate)
//
// Production host is Lua scripts/globals/besieged.lua:
//
//   xi.besieged.onEventUpdate (~185–195):
//     local entry = imperialStandingItems[option]
//     if not entry then return end
//     if option < 0x40000000 then
//       player:updateEvent(...)
//     end
//
//   xi.besieged.onEventFinish sanction branch (~202–219):
//     local sanctionCost = 100
//     if option == 0 then sanctionCost = 0 end
//     if imperialStanding < sanctionCost then return end
//
// Capacity is for future Lua/C++ inject so hosts dual-wire the pure gates
// instead of re-inlining the comparisons. Helpers take host-injected scalars
// only (no entity / currency pointers). updateEvent / delCurrency /
// addStatusEffect / messageSpecial writeback remains host-owned.
//
// Go dual-wire:
//   - ShouldUpdateStandingEvent → internal/besieged (flow.go)
//   - CanAffordSanction / SanctionAffordCost → internal/besieged/afford_sanction.go
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

// ---------------------------------------------------------------------------
// onEventFinish sanction afford (slice 2945)
// ---------------------------------------------------------------------------

// SanctionOptionNone is option == 0 (free afford cost; still debits 100).
constexpr int32 SanctionOptionNone = 0;

// SanctionStandingCost is the afford-check cost for non-zero sanction options
// (and the live delCurrency amount on success — always 100).
constexpr int32 SanctionStandingCost = 100;

// SanctionAffordCost is the imperial_standing amount used in the afford gate:
//
//   sanctionCost = 100
//   if option == 0: sanctionCost = 0
//
// Distinct from the live delCurrency debit (always 100 on success — LSB quirk).
// Dual-wires Go besieged.SanctionAffordCost (afford_sanction.go / 2945).
inline auto SanctionAffordCost(const int32 option) -> int32
{
    if (option == SanctionOptionNone)
    {
        return 0;
    }
    return SanctionStandingCost;
}

// CanAffordSanction is the pure free-function form of the onEventFinish
// sanction afford gate:
//
//   imperialStanding >= SanctionAffordCost(option)
//
// Positive form of the Lua reject `imperialStanding < sanctionCost`. Host
// injects option and getCurrency('imperial_standing') instead of re-inlining.
// Dual-wires Go besieged.CanAffordSanction (afford_sanction.go / 2945).
inline auto CanAffordSanction(const int32 option, const int32 imperialStanding) -> bool
{
    return imperialStanding >= SanctionAffordCost(option);
}

} // namespace besiegedhelpers
