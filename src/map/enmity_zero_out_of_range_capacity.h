#pragma once

#include "common/cbasetypes.h"

// Pure UpdateEnmity out-of-range CE/VE zero gate for dual-wire slices:
//   - 2927: ShouldZeroEnmityOutOfRange (withinRange → !withinRange)
//
// Production host today: CEnmityContainer::UpdateEnmity in enmity_container.cpp
// routes through enmitymath::ShouldZeroEnmityOutOfRange(IsWithinEnmityRange(PEntity))
// (same formula; residual pure port slice 1357 in enmity_container.h).
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/enmity (slice 1357 residual / 2927 dual-wire):
//
//   ShouldZeroEnmityOutOfRange(withinRange) = !withinRange
//
// Hosts inject the host-evaluated IsWithinEnmityRange boolean. Side effects
// (zeroing CE/VE deltas on the UpdateEnmity path) remain host-owned.
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
// Slice 2927 — ShouldZeroEnmityOutOfRange out-of-range CE/VE zero gate
// ---------------------------------------------------------------------------

// ShouldZeroEnmityOutOfRange mirrors !IsWithinEnmityRange → CE=VE=0.
//
// Formula (slice 2927 dual-wire):
//   ShouldZeroEnmityOutOfRange(withinRange) = !withinRange
//
// withinRange is host-evaluated IsWithinEnmityRange(PEntity)
// (same-zone && distance within EnmityRangeMax).
// true  → force CE=VE=0 on the UpdateEnmity path (out of range)
// false → leave CE/VE deltas as provided (within range)
//
// Dual-wire of Go enmity.ShouldZeroEnmityOutOfRange.
// Production call site: CEnmityContainer::UpdateEnmity (via enmitymath).
inline auto ShouldZeroEnmityOutOfRange(const bool withinRange) -> bool
{
    return !withinRange;
}

} // namespace enmityrangehelpers
