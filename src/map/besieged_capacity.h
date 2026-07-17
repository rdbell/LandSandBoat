#pragma once

#include "common/cbasetypes.h"

// Pure Besieged free-function dual-wire helpers shared by slices:
//   - 2859: ShouldUpdateStandingEvent / IsItemOptionPath residual dual-wire
//           (onEventUpdate gate)
//   - 3142: ShouldUpdateStandingEvent / IsItemOptionPath dedicated dual-wire
//           (flow.go; formula unchanged)
//   - 2945: CanAffordSanction / SanctionAffordCost residual dual-wire suite
//   - 3093: CanAffordSanction / SanctionAffordCost prior dedicated dual-wire
//           (afford_sanction.go)
//   - 3227: CanAffordSanction / SanctionAffordCost dedicated dual-wire
//           expand residual 2945 (afford_sanction.go; formula unchanged)
//
// Dual-wire index:
//   - 2859: ShouldUpdateStandingEvent residual (option < 0x40000000 item-path)
//   - 3142: ShouldUpdateStandingEvent (IsItemOptionPath / option < 0x40000000)
//   - 2945: CanAffordSanction residual dual-wire suite
//   - 3093: CanAffordSanction prior dedicated dual-wire suite
//   - 3227: CanAffordSanction (imperialStanding >= SanctionAffordCost(option))
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
//   - ShouldUpdateStandingEvent → internal/besieged (flow.go / 3142;
//     residual 2859)
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
//
// Formula (slice 3142 dual-wire; residual expand 2859):
//   ShouldUpdateStandingEvent(option) = IsItemOptionPath(option)
//   // option < 0x40000000   // ItemOptionCeiling
//
// Dual-wires IsItemOptionPath. Host looks up the catalog entry first; missing
// rows return early without calling this gate.
// Dual-wires Go besieged.ShouldUpdateStandingEvent (flow.go / 3142).
// Call site: future Lua onEventUpdate inject.
// Prior pure port: slice 1120. Residual dual-wire suite: 2859 /
// test_besieged_standing_event_2859. Dedicated dual-wire suite is
// test_besieged_standing_event_3142. Host still owns catalog lookup and
// updateEvent payload after a true gate.
// Sibling CanAffordSanction (3227 / 3093 / 2945) is independent (left alone).
inline auto ShouldUpdateStandingEvent(const uint32 option) -> bool
{
    return IsItemOptionPath(option);
}

// ---------------------------------------------------------------------------
// Slice 2945 / 3093 / 3227 — onEventFinish sanction afford
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
// Dual-wires Go besieged.SanctionAffordCost (afford_sanction.go / 3227
// dedicated expand residual 2945; prior dedicated 3093).
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
// Formula (slice 3227 dedicated dual-wire; residual expand 2945 / prior
// dedicated 3093 — formula unchanged):
//   CanAffordSanction(option, imperialStanding) =
//     imperialStanding >= SanctionAffordCost(option)
//
// Positive form of the Lua reject `imperialStanding < sanctionCost`. Host
// injects option and getCurrency('imperial_standing') instead of re-inlining.
// Dual-wires Go besieged.CanAffordSanction (afford_sanction.go / 3227).
// Call site: future Lua onEventFinish inject.
// Prior pure port: slice 1120. Residual dual-wire suite: 2945 /
// test_besieged_afford_sanction_2945. Prior dedicated dual-wire suite: 3093 /
// test_besieged_afford_sanction_3093. Dedicated dual-wire suite is
// test_besieged_afford_sanction_3227. Host still owns delCurrency /
// addStatusEffect / messageSpecial after a true gate.
// Note: afford cost for option 0 is free (0) but live debit is always 100.
inline auto CanAffordSanction(const int32 option, const int32 imperialStanding) -> bool
{
    return imperialStanding >= SanctionAffordCost(option);
}

} // namespace besiegedhelpers
