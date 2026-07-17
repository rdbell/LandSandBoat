#pragma once

#include "common/cbasetypes.h"

// Pure Nyzul Isle helpers shared by dual-wire slices:
//   - 2874: free-floor selection gate (pickSetPoint)
//
// Production host is Lua under
// scripts/zones/Nyzul_Isle/instances/nyzul_isle_investigation.lua
// local pickSetPoint:
//
//   elseif math.random(1, 30) == 1 and instance:getLocalVar('freeFloor') == 0 then
//     instance:setStage(xi.nyzul.objective.FREE_FLOOR)
//     instance:setLocalVar('freeFloor', 1)
//     -- timer(9000) → setProgress(15)
//
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining the roll/localVar comparison. Helpers
// take host-injected scalars only (no instance / entity / NPC pointers).
// Side effects (setStage FREE_FLOOR, freeFloor localVar, Rune of Transfer
// timer / setProgress) remain host-owned.
// Prior pure port: OmegaXI slice 1088 (internal/nyzul floorflow).

namespace nyzulhelpers
{

// ---------------------------------------------------------------------------
// Slice 2874 — pickSetPoint free-floor selection gate
// ---------------------------------------------------------------------------

// FreeFloorRollHit is the free-floor success value for math.random(1, 30)
// (== 1 → ~3.33%).
inline constexpr int32 FreeFloorRollHit = 1;

// ShouldGrantFreeFloor mirrors pickSetPoint free-floor selection:
//   math.random(1, 30) == 1 and freeFloor == 0
// roll1to30 is the host-injected math.random(1, 30) result.
// freeFloorVar is instance:getLocalVar('freeFloor') (0 = never granted this
// run). Host still owns setStage / freeFloor writeback / timer.
inline auto ShouldGrantFreeFloor(const int32 roll1to30, const int32 freeFloorVar) -> bool
{
    return freeFloorVar == 0 && roll1to30 == FreeFloorRollHit;
}

} // namespace nyzulhelpers
