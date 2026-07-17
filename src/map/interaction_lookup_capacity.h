#pragma once

#include "common/cbasetypes.h"

// Pure interaction_lookup helpers shared by dual-wire slices:
//   - 2953: ShouldSkipPrioritySelection (getHighestPriorityActions early
//           return gate — empty list or bare-number CS ID first element)
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
// handler_policy.go). Dual-wire extract: slice 2953 (skip_priority.go).

namespace interactionlookuphelpers
{

// ---------------------------------------------------------------------------
// Slice 2953 — getHighestPriorityActions early-return gate
// ---------------------------------------------------------------------------

// SkipPriorityMaxPriority is the maxPriority returned when
// ShouldSkipPrioritySelection is true (Action.Priority.Progress == 1000).
// Mirrors Go interactionlookup.SkipPriorityMaxPriority /
// interactionaction.PriorityProgress.
inline constexpr int32 SkipPriorityMaxPriority = 1000;

// ShouldSkipPrioritySelection mirrors getHighestPriorityActions early return:
//   empty || firstIsNumber
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
inline auto ShouldSkipPrioritySelection(const bool empty, const bool firstIsNumber) -> bool
{
    return empty || firstIsNumber;
}

} // namespace interactionlookuphelpers
