#pragma once

#include "common/cbasetypes.h"

// Pure interaction_lookup helpers shared by dual-wire slices:
//   - 1094: residual pure port (internal/interactionlookup ShouldSkipPrioritySelection)
//   - 2953: ShouldSkipPrioritySelection residual dual-wire extract (skip_priority)
//   - 3167: ShouldSkipPrioritySelection prior dedicated dual-wire
//           (empty || firstIsNumber; residual expand 2953 / pure 1094)
//   - 3246: ShouldSkipPrioritySelection prior dedicated dual-wire
//           (empty || firstIsNumber; residual expand 2953 / prior 3167)
//   - 3332: ShouldSkipPrioritySelection dedicated dual-wire
//           (empty || firstIsNumber; residual expand 2953 / prior 3246 / 3167)
//
// Dual-wire index:
//   - 2953: ShouldSkipPrioritySelection residual dual-wire suite
//   - 3167: ShouldSkipPrioritySelection prior dedicated dual-wire suite
//   - 3246: ShouldSkipPrioritySelection prior dedicated dual-wire suite
//   - 3332: ShouldSkipPrioritySelection = empty || firstIsNumber
//
// Production host is Lua under
// scripts/globals/interaction/interaction_lookup.lua
// local getHighestPriorityActions:
//
//   if
//       possibleActions and
//       #possibleActions == 0 or
//       type(possibleActions[1]) == 'number'
//   then
//       return possibleActions, Action.Priority.Progress
//   end
//
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining empty/firstIsNumber comparisons. Helpers
// take host-injected scalars only (no player / data / handler table pointers).
// Side effects (returning the raw possibleActions list, skipping
// HighestPriorityActions selection, Action.Priority.Progress as maxPriority)
// remain host-owned.
// Prior pure port: OmegaXI slice 1094 (internal/interactionlookup
// handler_policy.go). Residual dual-wire extract: slice 2953 (skip_priority.go).
// Prior dedicated dual-wire suites: slices 3167 / 3246
// (test_interactionlookup_skip_priority_3167 /
// test_interactionlookup_skip_priority_3246).
// Dedicated dual-wire suite: slice 3332
// (test_interactionlookup_skip_priority_3332).
// Go dual-wire: interactionlookup.ShouldSkipPrioritySelection
//   (internal/interactionlookup/skip_priority.go).

namespace interactionlookuphelpers
{

// ---------------------------------------------------------------------------
// Slice 3332 — getHighestPriorityActions early-return gate
//              (dedicated expand residual 2953; prior 3246 / 3167)
// ---------------------------------------------------------------------------

// SkipPriorityMaxPriority is the maxPriority returned when
// ShouldSkipPrioritySelection is true (Action.Priority.Progress == 1000).
// Mirrors Go interactionlookup.SkipPriorityMaxPriority /
// interactionaction.PriorityProgress.
// Residual pin under 2953 / 1094; prior dedicated dual-wire suites: slices 3167 / 3246;
// dedicated dual-wire suite: slice 3332.
inline constexpr int32 SkipPriorityMaxPriority = 1000;

// ShouldSkipPrioritySelection mirrors getHighestPriorityActions early return:
//   empty || firstIsNumber
//
// Formula (slice 3332 dedicated dual-wire; residual expand 2953 / prior 3246 /
// 3167 / pure 1094 — formula unchanged):
//   ShouldSkipPrioritySelection(empty, firstIsNumber) = empty || firstIsNumber
//
// empty is #possibleActions == 0; firstIsNumber is
// type(possibleActions[1]) == 'number' (onZoneIn CS ID path before
// ActionCandidate conversion).
//
// When true, the host should return the raw list with maxPriority Progress
// rather than running HighestPriorityActions.
//
// Note: Lua operator precedence is
// (possibleActions and #==0) or type([1])=='number'.
// Capacity models the effective gate as empty || firstIsNumber once
// possibleActions is known.
//
// Dual-wire of Go interactionlookup.ShouldSkipPrioritySelection
// (internal/interactionlookup/skip_priority.go).
// Call site: future Lua/C++ host of getHighestPriorityActions.
// Prior pure port: slice 1094. Residual dual-wire suite: 2953 /
// test_interaction_skip_priority_2953. Prior dedicated dual-wire suites:
// test_interactionlookup_skip_priority_3167 /
// test_interactionlookup_skip_priority_3246. Dedicated dual-wire suite is
// test_interactionlookup_skip_priority_3332. Formula is unchanged; this slice
// only expands dual-wire docs + index + dedicated suite. Future Lua host
// injects empty / firstIsNumber into this helper instead of re-inlining.
inline auto ShouldSkipPrioritySelection(const bool empty, const bool firstIsNumber) -> bool
{
    return empty || firstIsNumber;
}

} // namespace interactionlookuphelpers
