#pragma once

#include "common/cbasetypes.h"

// Pure UpdateEnmity out-of-range CE/VE zero gate for dual-wire slices:
//   - 1357: residual pure port (enmitymath in enmity_container.h)
//   - 2927: ShouldZeroEnmityOutOfRange residual dual-wire suite
//   - 3157: ShouldZeroEnmityOutOfRange dedicated dual-wire (zero_out_of_range.go)
//
// Dual-wire index:
//   - 2927: ShouldZeroEnmityOutOfRange residual dual-wire suite
//   - 3157: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//
// Production host today: CEnmityContainer::UpdateEnmity in enmity_container.cpp
// routes through enmitymath::ShouldZeroEnmityOutOfRange(IsWithinEnmityRange(PEntity))
// (same formula; residual pure port slice 1357 in enmity_container.h).
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/enmity (zero_out_of_range.go) so hosts call
// ShouldZeroEnmityOutOfRange instead of re-inlining !withinRange, and so
// production can inject:
//
//   ShouldZeroEnmityOutOfRange(IsWithinEnmityRange(PEntity))
//
// Hosts inject the host-evaluated IsWithinEnmityRange boolean. Side effects
// (zeroing CE/VE deltas on the UpdateEnmity path) remain host-owned.
//
// Go dual-wire: enmity.ShouldZeroEnmityOutOfRange (internal/enmity/zero_out_of_range.go).
// Residual dual-wire suite: 2927.
// Dedicated dual-wire suite: 3157.
// Prior pure port: OmegaXI slice 1357 (internal/enmity).
//
// Reference: src/map/enmity_container.cpp CEnmityContainer::UpdateEnmity
//   if (enmitymath::ShouldZeroEnmityOutOfRange(IsWithinEnmityRange(PEntity)))
//   {
//       CE = 0;
//       VE = 0;
//   }
//
// Reference: src/map/enmity_container.h enmitymath::ShouldZeroEnmityOutOfRange
//   return !withinRange;

namespace enmityrangehelpers
{

// ---------------------------------------------------------------------------
// Slice 3157 — ShouldZeroEnmityOutOfRange out-of-range CE/VE zero gate
// (dedicated dual-wire; residual expand 2927 / pure 1357 — formula unchanged)
// ---------------------------------------------------------------------------

// ShouldZeroEnmityOutOfRange mirrors !IsWithinEnmityRange → CE=VE=0.
//
// Formula (slice 3157 dedicated dual-wire; residual expand 2927 / pure 1357 —
// formula unchanged):
//   ShouldZeroEnmityOutOfRange(withinRange) = !withinRange
//
// withinRange is host-evaluated IsWithinEnmityRange(PEntity)
// (same-zone && distance within EnmityRangeMax).
// true  → force CE=VE=0 on the UpdateEnmity path (out of range)
// false → leave CE/VE deltas as provided (within range)
//
// Dual-wire of Go enmity.ShouldZeroEnmityOutOfRange.
// Production call site: CEnmityContainer::UpdateEnmity (via enmitymath).
// Residual dual-wire suite: 2927. Dedicated dual-wire suite: 3157.
inline auto ShouldZeroEnmityOutOfRange(const bool withinRange) -> bool
{
    return !withinRange;
}

} // namespace enmityrangehelpers
