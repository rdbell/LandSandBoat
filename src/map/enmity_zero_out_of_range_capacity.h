#pragma once

#include "common/cbasetypes.h"

// Pure UpdateEnmity out-of-range CE/VE zero gate for dual-wire slices:
//   - 1357: residual pure port (enmitymath in enmity_container.h)
//   - 2927: ShouldZeroEnmityOutOfRange residual dual-wire suite
//   - 3157: ShouldZeroEnmityOutOfRange prior dedicated dual-wire
//           (zero_out_of_range.go)
//   - 3333: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated 3157; formula unchanged)
//   - 3427: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3333 / prior dedicated 3157; formula unchanged)
//   - 3485: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3427 / 3333 / prior dedicated 3157; formula unchanged)
//   - 3545: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3485 / 3427 / 3333 / prior dedicated 3157; formula unchanged)
//   - 3589: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3545 / 3485 / 3427 / 3333 / prior dedicated 3157; formula unchanged)
//   - 3634: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157; formula unchanged)
//   - 3679: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157; formula unchanged)
//   - 3724: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157; formula unchanged)
//   - 3769: ShouldZeroEnmityOutOfRange prior dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157; formula unchanged)
//   - 3814: ShouldZeroEnmityOutOfRange dedicated dual-wire expand residual 2927
//           (prior dedicated expand 3769 / 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157; formula unchanged)
//
// Dual-wire index:
//   - 2927: ShouldZeroEnmityOutOfRange residual dual-wire suite
//   - 3157: ShouldZeroEnmityOutOfRange prior dedicated dual-wire
//   - 3333: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated 3157)
//   - 3427: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3333 /
//     prior dedicated 3157)
//   - 3485: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3427 /
//     prior dedicated expand 3333 / prior dedicated 3157)
//   - 3545: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3485 /
//     prior dedicated expand 3427 / prior dedicated expand 3333 / prior dedicated 3157)
//   - 3589: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3545 /
//     prior dedicated expand 3485 / prior dedicated expand 3427 / prior dedicated expand 3333 /
//     prior dedicated 3157)
//   - 3634: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3589 /
//     prior dedicated expand 3545 / prior dedicated expand 3485 / prior dedicated expand 3427 /
//     prior dedicated expand 3333 / prior dedicated 3157)
//   - 3679: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3634 /
//     prior dedicated expand 3589 / prior dedicated expand 3545 / prior dedicated expand 3485 /
//     prior dedicated expand 3427 / prior dedicated expand 3333 / prior dedicated 3157)
//   - 3724: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3679 /
//     prior dedicated expand 3634 / prior dedicated expand 3589 / prior dedicated expand 3545 /
//     prior dedicated expand 3485 / prior dedicated expand 3427 / prior dedicated expand 3333 /
//     prior dedicated 3157)
//   - 3769: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     prior dedicated dual-wire expand residual 2927 (prior dedicated expand 3724 /
//     prior dedicated expand 3679 / prior dedicated expand 3634 / prior dedicated expand 3589 /
//     prior dedicated expand 3545 / prior dedicated expand 3485 / prior dedicated expand 3427 /
//     prior dedicated expand 3333 / prior dedicated 3157)
//   - 3814: ShouldZeroEnmityOutOfRange = !withinRange (identity-not)
//     dedicated dual-wire expand residual 2927 (prior dedicated expand 3769 /
//     prior dedicated expand 3724 / prior dedicated expand 3679 / prior dedicated expand 3634 /
//     prior dedicated expand 3589 / prior dedicated expand 3545 / prior dedicated expand 3485 /
//     prior dedicated expand 3427 / prior dedicated expand 3333 / prior dedicated 3157)
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
// Prior dedicated dual-wire suite: 3157.
// Prior dedicated dual-wire expand residual suite: 3333.
// Prior dedicated dual-wire expand residual suite: 3427.
// Prior dedicated dual-wire expand residual suite: 3485.
// Prior dedicated dual-wire expand residual suite: 3545.
// Prior dedicated dual-wire expand residual suite: 3589.
// Prior dedicated dual-wire expand residual suite: 3634.
// Prior dedicated dual-wire expand residual suite: 3679.
// Prior dedicated dual-wire expand residual suite: 3724.
// Prior dedicated dual-wire expand residual suite: 3769.
// Dedicated dual-wire expand residual: 3814 (residual 2927 / prior 3769 / 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / 3157).
// Prior pure port: OmegaXI slice 1357 (internal/enmity).
//
// Coverage: test_enmity_zero_out_of_range_2927 (residual),
// test_enmity_zero_out_of_range_3157 (prior dedicated dual-wire; not in CMake/main),
// test_enmity_zero_out_of_range_3333 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3427 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3485 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3545 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3589 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3634 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3679 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3724 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3769 (prior dedicated expand residual 2927; not in CMake/main),
// test_enmity_zero_out_of_range_3814 (dedicated expand residual 2927; not in CMake/main).
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
// Slice 2927 residual / 3157 prior dedicated / 3333 prior expand residual 2927
// / 3427 prior expand residual 2927 / 3485 prior expand residual 2927
// / 3545 prior expand residual 2927 / 3589 prior expand residual 2927
// / 3634 prior expand residual 2927 / 3679 prior expand residual 2927
// / 3724 prior expand residual 2927 / 3769 prior expand residual 2927
// / 3814 dedicated expand residual 2927
// — ShouldZeroEnmityOutOfRange out-of-range CE/VE zero gate
// (dedicated dual-wire expand residual 2927; prior dedicated expand 3769 /
// 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157 / pure 1357 — formula unchanged)
// ---------------------------------------------------------------------------

// ShouldZeroEnmityOutOfRange mirrors !IsWithinEnmityRange → CE=VE=0.
//
// Formula (slice 3814 dedicated dual-wire expand residual 2927; prior
// dedicated expand 3769 / 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157 / pure 1357 —
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
// Residual dual-wire suite: 2927 / test_enmity_zero_out_of_range_2927.
// Prior dedicated dual-wire suite is test_enmity_zero_out_of_range_3157.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3333.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3427.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3485.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3545.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3589.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3634.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3679.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3724.
// Prior dedicated expand residual suite is test_enmity_zero_out_of_range_3769.
// Dedicated expand residual suite is test_enmity_zero_out_of_range_3814.
// Host still owns zone + distance evaluation and CE/VE zero writeback.
// Coverage: test_enmity_zero_out_of_range_3814 (not in CMake/main); residual 2927 /
// prior dedicated expand 3769 / 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157 retained.
inline auto ShouldZeroEnmityOutOfRange(const bool withinRange) -> bool
{
    return !withinRange;
}

} // namespace enmityrangehelpers
