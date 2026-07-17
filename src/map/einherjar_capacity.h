#pragma once

#include "common/cbasetypes.h"

// Pure Einherjar helpers shared by dual-wire slices:
//   - 2864: onMobDespawn cycleWave gate (remaining live mobs)
//   - 2865: emptyChamberCheck expel gate (player count)
//
// Production hosts are Lua under scripts/globals/einherjar/system.lua.
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining comparisons. Helpers take host-injected
// scalars only (no entity / chamber / mob / player pointers).
// Side effects (cycleWave spawn, armoury crate, special despawn,
// expelAllFromChamber, log) remain host-owned.

namespace einherjarhelpers
{

// ---------------------------------------------------------------------------
// Slice 2864 — onMobDespawn cycleWave gate
// ---------------------------------------------------------------------------

// ShouldCycleWave mirrors local onMobDespawn after active-list removal:
//   if #chamberData.mobs <= 0 then
//     xi.einherjar.cycleWave(chamberData)
//   end
// remainingMobs is the host-injected live-mob count after the despawned
// entry is removed (Lua #chamberData.mobs). Host still calls cycleWave().
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
