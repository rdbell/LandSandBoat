#pragma once

#include "common/cbasetypes.h"

// Pure Clamming helpers for dual-wire slices:
//   - 2869: kit weight add / bucket overload gate (residual dual-wire expand)
//   - 3180: BucketOverloaded dedicated dual-wire
//            (residual expand 2869 / pure 1013 — formula unchanged)
//
// Production host is Lua under scripts/globals/hobbies/clamming/logic.lua
// (nodeOnEventUpdate weight check). Capacity is for future Lua/C++ inject so
// hosts dual-wire pure free functions instead of re-inlining
// kitWeight + itemWeight > kitSize. Helpers take host-injected scalars only
// (no entity / player / NPC pointers). Side effects (KitBroken char var,
// emptyBucket, local-var item counts, KitWeight char-var writeback) remain
// host-owned.
//
// Residual dual-wire suite: 2869 / test_clamming_bucket_overload_2869.
// Dedicated dual-wire suite: 3180 / test_clamming_bucket_overloaded_3180.

namespace clamminghelpers
{

// ---------------------------------------------------------------------------
// Slice 3180 — clamming kit weight / bucket overload gate
// (dedicated dual-wire; residual expand 2869 / pure 1013 — formula unchanged)
// ---------------------------------------------------------------------------

// TotalWeight mirrors the always-run kit weight update after a clam roll:
//   player:setCharVar('[Clam]KitWeight', kitWeight + itemWeight)
// Host still performs the char-var writeback.
// Formula (slice 3180 dedicated dual-wire; residual expand 2869 / pure 1013):
//   kitWeight + itemWeight
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
// Formula (slice 3180 dedicated dual-wire; residual expand 2869 / pure 1013 —
// formula unchanged):
//   TotalWeight(kitWeight, itemWeight) > kitSize
// Residual dual-wire suite: 2869 / test_clamming_bucket_overload_2869.
// Dedicated dual-wire suite is test_clamming_bucket_overloaded_3180. Formula is
// unchanged; this slice only expands dual-wire docs + index + dedicated suite.
inline auto BucketOverloaded(const int32 kitWeight, const int32 itemWeight, const int32 kitSize) -> bool
{
    return TotalWeight(kitWeight, itemWeight) > kitSize;
}

} // namespace clamminghelpers
