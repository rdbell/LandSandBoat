#pragma once

#include "common/cbasetypes.h"

// Pure Monstrosity helpers shared by dual-wire slices:
//   - 2872: ShouldRejectInstinctLoadout (instinct equip cost/duplicate gate)
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

namespace monstrosityhelpers
{

// ShouldRejectInstinctLoadout is the pure free-function form of the
// HandleEquipChangePacket cost/duplicate reject gate:
//   hasDuplicates || totalCost > maxPoints
// Exact equality totalCost == maxPoints is accepted (not rejected).
// Future host injects scalars into this helper instead of re-inlining the OR.
inline auto ShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return hasDuplicates || totalCost > maxPoints;
}

} // namespace monstrosityhelpers
