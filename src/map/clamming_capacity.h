#pragma once

#include "common/cbasetypes.h"

// Pure Clamming helpers for dual-wire slices:
//   - 2869: kit weight add / bucket overload gate
//
// Production host is Lua under scripts/globals/hobbies/clamming/logic.lua
// (nodeOnEventUpdate weight check). Capacity is for future Lua/C++ inject so
// hosts dual-wire pure free functions instead of re-inlining
// kitWeight + itemWeight > kitSize. Helpers take host-injected scalars only
// (no entity / player / NPC pointers). Side effects (KitBroken char var,
// emptyBucket, local-var item counts, KitWeight char-var writeback) remain
// host-owned.

namespace clamminghelpers
{

// ---------------------------------------------------------------------------
// Slice 2869 — clamming kit weight / bucket overload gate
// ---------------------------------------------------------------------------

// TotalWeight mirrors the always-run kit weight update after a clam roll:
//   player:setCharVar('[Clam]KitWeight', kitWeight + itemWeight)
// Host still performs the char-var writeback.
inline auto TotalWeight(const int32 kitWeight, const int32 itemWeight) -> int32
{
    return kitWeight + itemWeight;
}

// BucketOverloaded mirrors the nodeOnEventUpdate weight limit check:
//   if kitWeight + itemWeight > kitSize then
//     KitBroken + emptyBucket
//   else
//     add item to bucket locals
//   end
// Strict greater-than: filling exactly to capacity is allowed. Host still
// owns KitBroken / emptyBucket / item local-var writeback and the always-run
// KitWeight update (TotalWeight).
inline auto BucketOverloaded(const int32 kitWeight, const int32 itemWeight, const int32 kitSize) -> bool
{
    return TotalWeight(kitWeight, itemWeight) > kitSize;
}

} // namespace clamminghelpers
