#pragma once

#include "common/cbasetypes.h"

// Pure Monstrosity helpers shared by dual-wire slices:
//   - 2766: residual pure port (internal/monstrosity ShouldRejectInstinctLoadout)
//   - 2872: ShouldRejectInstinctLoadout residual dual-wire suite (instinct_loadout)
//   - 3168: ShouldRejectInstinctLoadout dedicated dual-wire (instinct_equip.go)
//
// Dual-wire index:
//   - 2872: ShouldRejectInstinctLoadout residual dual-wire suite
//   - 3168: ShouldRejectInstinctLoadout = hasDuplicates || totalCost > maxPoints
//
// Production host (2766 / map): src/map/monstrosity.cpp
//   monstrosity::HandleEquipChangePacket InstinctFlag path:
//
//   const auto totalCost = TotalInstinctCost(resolveInstinctCosts(...));
//   if (ShouldRejectInstinctLoadout(totalCost, maxPoints,
//                                   HasDuplicateInstincts(...)))
//   {
//       // reset EquippedInstincts to previousEquipped; skip mods
//   }
//
// Prior pure surface (namespace monstrosity): monstrosity_instinct_equip.h
// (slice 2766). This capacity free function dual-wires the same reject gate
// under monstrosityhelpers for inject parity with Go
// internal/monstrosity.ShouldRejectInstinctLoadout.
//
// Host injects scalars only (no char / catalog / slot array pointers):
//   totalCost      — host-resolved sum of equipped instinct costs
//   maxPoints      — InstinctMaxPoints(level) = level + 10
//   hasDuplicates  — host-resolved HasDuplicateInstincts over 12 slots
//
// Catalog cost resolution, unlock checks, and modifier apply/reset remain
// host-owned.
// Go dual-wire: monstrosity.ShouldRejectInstinctLoadout
//   (internal/monstrosity/instinct_equip.go).
// Residual dual-wire suite: 2872 (test_monstrosity_instinct_loadout_2872).
// Dedicated dual-wire suite: 3168 (test_monstrosity_reject_instinct_loadout_3168).
// Prior pure port: OmegaXI slice 2766 (internal/monstrosity).

namespace monstrosityhelpers
{

// ---------------------------------------------------------------------------
// Slice 3168 — HandleEquipChangePacket cost/duplicate reject gate
// (dedicated expand residual 2872)
// ---------------------------------------------------------------------------

// ShouldRejectInstinctLoadout is the pure free-function form of the
// HandleEquipChangePacket cost/duplicate reject gate:
//
//   hasDuplicates || totalCost > maxPoints
//
// Formula (slice 3168 dedicated dual-wire; residual expand 2872 / pure 2766 —
// formula unchanged):
//   ShouldRejectInstinctLoadout(totalCost, maxPoints, hasDuplicates)
//     = hasDuplicates || totalCost > maxPoints
//
// Exact equality totalCost == maxPoints is accepted (not rejected).
// Host injects scalars only. Catalog cost resolution, unlock checks,
// HasDuplicateInstincts / TotalInstinctCost / InstinctMaxPoints, and
// reset-to-previous remain host-owned.
// Dual-wire of Go monstrosity.ShouldRejectInstinctLoadout.
// Call site: future host inject (HandleEquipChangePacket InstinctFlag path).
// Prior pure port: slice 2766. Residual dual-wire suite: 2872 /
// test_monstrosity_instinct_loadout_2872. Dedicated dual-wire suite is
// test_monstrosity_reject_instinct_loadout_3168. Future host injects scalars
// into this helper instead of re-inlining the OR.
inline auto ShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return hasDuplicates || totalCost > maxPoints;
}

} // namespace monstrosityhelpers
