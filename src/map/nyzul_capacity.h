#pragma once

#include "common/cbasetypes.h"

// Pure Nyzul Isle helpers shared by dual-wire slices:
//   - 2874: free-floor selection gate (pickSetPoint)
//   - 2891: gear-objective chance gate (pickSetPoint)
//
// Production host is Lua under
// scripts/zones/Nyzul_Isle/instances/nyzul_isle_investigation.lua
// local pickSetPoint:
//
//   elseif math.random(1, 30) == 1 and instance:getLocalVar('freeFloor') == 0 then
//     instance:setStage(xi.nyzul.objective.FREE_FLOOR)
//     instance:setLocalVar('freeFloor', 1)
//     -- timer(9000) → setProgress(15)
//   ...
//   if math.random(1, 30) <= 5 then
//     instance:setLocalVar('gearObjective',
//       math.random(xi.nyzul.gearObjective.AVOID_AGRO,
//                   xi.nyzul.gearObjective.DO_NOT_DESTROY))
//   end
//
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining roll/localVar comparisons. Helpers
// take host-injected scalars only (no instance / entity / NPC pointers).
// Side effects (setStage FREE_FLOOR, freeFloor / gearObjective localVar,
// Rune of Transfer timer / setProgress, gear objective type pick) remain
// host-owned.
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

// ---------------------------------------------------------------------------
// Slice 2891 — pickSetPoint gear-objective chance gate
// ---------------------------------------------------------------------------

// GearObjectiveRollThreshold is the gear success ceiling for math.random(1, 30)
// (roll ≤ 5 → ~16.7%).
inline constexpr int32 GearObjectiveRollThreshold = 5;

// ShouldRollGearObjective mirrors pickSetPoint gear-objective chance:
//   math.random(1, 30) <= 5
// Implemented as roll >= 1 && roll <= threshold so out-of-range rolls do not
// spuriously succeed. roll1to30 is the host-injected math.random(1, 30)
// result. Host still owns gearObjective localVar writeback and the
// AVOID_AGRO..DO_NOT_DESTROY pick. Boss / free-floor branches short-circuit
// before this gate in Lua; host still owns that branch order.
inline auto ShouldRollGearObjective(const int32 roll1to30) -> bool
{
    return roll1to30 >= 1 && roll1to30 <= GearObjectiveRollThreshold;
}

} // namespace nyzulhelpers
