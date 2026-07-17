#pragma once

#include "common/cbasetypes.h"

// Pure Einherjar helpers shared by dual-wire slices:
//   - 1069: residual pure port (internal/einherjar system helpers)
//   - 2864: onMobDespawn cycleWave gate residual dual-wire suite
//   - 2865: emptyChamberCheck expel gate residual dual-wire suite
//   - 3181: ShouldCycleWave prior dedicated dual-wire (cycle_wave.go;
//           expand residual 2864; suite retained)
//   - 3212: ShouldEmptyChamberTimeout dedicated dual-wire (empty_timeout.go;
//           expand residual 2865)
//   - 3329: ShouldCycleWave prior dedicated dual-wire (cycle_wave.go;
//           expand residual 2864; prior dedicated 3181 retained; suite retained)
//   - 3517: ShouldCycleWave dedicated dual-wire (cycle_wave.go;
//           expand residual 2864; prior dedicated 3181 / 3329 retained)
//
// Dual-wire index:
//   - 2864: ShouldCycleWave residual dual-wire suite
//   - 2865: ShouldEmptyChamberTimeout residual dual-wire suite
//   - 3181: ShouldCycleWave prior dedicated dual-wire expand residual 2864
//   - 3212: ShouldEmptyChamberTimeout = playerCount == 0
//   - 3329: ShouldCycleWave prior dedicated dual-wire expand residual 2864
//   - 3517: ShouldCycleWave = remainingMobs <= 0
//           dedicated dual-wire expand residual 2864
//
// Production hosts are Lua under scripts/globals/einherjar/system.lua.
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining comparisons. Helpers take host-injected
// scalars only (no entity / chamber / mob / player pointers).
// Side effects (cycleWave spawn, armoury crate, special despawn,
// expelAllFromChamber, log) remain host-owned.
//
// Go dual-wire: einherjar.ShouldCycleWave
// (internal/einherjar/cycle_wave.go; slice 3517).
// Residual dual-wire suite: 2864 (test_einherjar_cycle_wave_2864).
// Prior dedicated dual-wire suites: 3181 / 3329
// (test_einherjar_should_cycle_wave_3181;
//  test_einherjar_should_cycle_wave_3329; retained).
// Dedicated dual-wire suite: 3517
// (test_einherjar_should_cycle_wave_3517; not in CMake/main).
//
// Go dual-wire: einherjar.ShouldEmptyChamberTimeout
// (internal/einherjar/empty_timeout.go).
// Residual dual-wire suite: 2865 (test_einherjar_empty_timeout_2865).
// Dedicated dual-wire suite: 3212 (test_einherjar_empty_chamber_timeout_3212).

namespace einherjarhelpers
{

// ---------------------------------------------------------------------------
// Slice 3517 — onMobDespawn cycleWave gate (dedicated expand residual 2864)
// ---------------------------------------------------------------------------

// ShouldCycleWave mirrors local onMobDespawn after active-list removal:
//   if #chamberData.mobs <= 0 then
//     xi.einherjar.cycleWave(chamberData)
//   end
//
// Formula (slice 3517 dedicated dual-wire; residual expand 2864 / pure 1069 /
// prior dedicated 3181 / 3329 — formula unchanged):
//
//   remainingMobs <= 0
//
// remainingMobs is the host-injected live-mob count after the despawned
// entry is removed (Lua #chamberData.mobs). Host still calls cycleWave().
// Matches Go einherjar.ShouldCycleWave (residual 1069 / residual dual-wire
// 2864 / prior dedicated dual-wire 3181 / 3329 / dedicated dual-wire 3517).
//
// Dual-wire of Go einherjar.ShouldCycleWave.
// Prior pure port: slice 1069. Residual dual-wire suite: 2864 /
// test_einherjar_cycle_wave_2864. Prior dedicated dual-wire suites:
// 3181 / test_einherjar_should_cycle_wave_3181 (retained);
// 3329 / test_einherjar_should_cycle_wave_3329 (retained). Dedicated
// dual-wire suite is test_einherjar_should_cycle_wave_3517.
// Dual-wire notes (slice 3517): Formula unchanged; dedicated suite expands
// free == inline == pin (direct remainingMobs <= 0) + residual 1069/2864
// pins + prior 3181 / 3329 poles + dense remaining range + prior 3329
// independence. Residual 2864 / prior 3181 / 3329 suites retained.
inline auto ShouldCycleWave(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

// ---------------------------------------------------------------------------
// Slice 3212 — emptyChamberCheck expel gate (dedicated expand residual 2865)
// ---------------------------------------------------------------------------

// ShouldEmptyChamberTimeout mirrors local emptyChamberCheck:
//   if playersCount(chamberData.players) == 0 then
//     log(...); expelAllFromChamber(chamberData)
//   end
//
// Formula (slice 3212 dedicated dual-wire; residual expand 2865 / pure 1069 —
// formula unchanged):
//
//   playerCount == 0
//
// playerCount is the host-injected map/list size (PlayersCount /
// PlayersListCount). Host still owns log + expel writeback.
// Matches Go einherjar.ShouldEmptyChamberTimeout (residual 1069 / residual
// dual-wire 2865 / dedicated dual-wire 3212).
//
// Dual-wire of Go einherjar.ShouldEmptyChamberTimeout.
// Prior pure port: slice 1069. Residual dual-wire suite: 2865 /
// test_einherjar_empty_timeout_2865. Dedicated dual-wire suite is
// test_einherjar_empty_chamber_timeout_3212.
inline auto ShouldEmptyChamberTimeout(const int32 playerCount) -> bool
{
    return playerCount == 0;
}

} // namespace einherjarhelpers
