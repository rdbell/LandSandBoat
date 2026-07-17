#pragma once

#include "common/cbasetypes.h"

// Pure Einherjar helpers shared by dual-wire slices:
//   - 1069: residual pure port (internal/einherjar system helpers)
//   - 2864: onMobDespawn cycleWave gate residual dual-wire suite
//   - 2865: emptyChamberCheck expel gate (player count)
//   - 3181: ShouldCycleWave dedicated dual-wire (cycle_wave.go;
//           expand residual 2864)
//
// Dual-wire index:
//   - 2864: ShouldCycleWave residual dual-wire suite
//   - 2865: ShouldEmptyChamberTimeout
//   - 3181: ShouldCycleWave = remainingMobs <= 0
//
// Production hosts are Lua under scripts/globals/einherjar/system.lua.
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining comparisons. Helpers take host-injected
// scalars only (no entity / chamber / mob / player pointers).
// Side effects (cycleWave spawn, armoury crate, special despawn,
// expelAllFromChamber, log) remain host-owned.
//
// Go dual-wire: einherjar.ShouldCycleWave
// (internal/einherjar/cycle_wave.go).
// Residual dual-wire suite: 2864 (test_einherjar_cycle_wave_2864).
// Dedicated dual-wire suite: 3181 (test_einherjar_should_cycle_wave_3181).

namespace einherjarhelpers
{

// ---------------------------------------------------------------------------
// Slice 3181 — onMobDespawn cycleWave gate (dedicated expand residual 2864)
// ---------------------------------------------------------------------------

// ShouldCycleWave mirrors local onMobDespawn after active-list removal:
//   if #chamberData.mobs <= 0 then
//     xi.einherjar.cycleWave(chamberData)
//   end
//
// Formula (slice 3181 dedicated dual-wire; residual expand 2864 / pure 1069 —
// formula unchanged):
//
//   remainingMobs <= 0
//
// remainingMobs is the host-injected live-mob count after the despawned
// entry is removed (Lua #chamberData.mobs). Host still calls cycleWave().
// Matches Go einherjar.ShouldCycleWave (residual 1069 / residual dual-wire
// 2864 / dedicated dual-wire 3181).
//
// Dual-wire of Go einherjar.ShouldCycleWave.
// Prior pure port: slice 1069. Residual dual-wire suite: 2864 /
// test_einherjar_cycle_wave_2864. Dedicated dual-wire suite is
// test_einherjar_should_cycle_wave_3181.
inline auto ShouldCycleWave(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

// ---------------------------------------------------------------------------
// Slice 2865 — emptyChamberCheck expel gate
// ---------------------------------------------------------------------------

// ShouldEmptyChamberTimeout mirrors local emptyChamberCheck:
//   if playersCount(chamberData.players) == 0 then
//     log(...); expelAllFromChamber(chamberData)
//   end
// playerCount is the host-injected map/list size (PlayersCount /
// PlayersListCount). Host still owns log + expel writeback.
inline auto ShouldEmptyChamberTimeout(const int32 playerCount) -> bool
{
    return playerCount == 0;
}

} // namespace einherjarhelpers
