#pragma once

#include "common/cbasetypes.h"

// Pure Garrison BATTLE fail-by-death gate helpers.
//
// Production host is Lua scripts/globals/garrison.lua BATTLE state
// (~420–434). Capacity is for future Lua/C++ inject so hosts dual-wire
// pure free functions instead of re-inlining comparisons. Helpers take
// host-injected scalars only (no entity / zone / player pointers).
// Side effects (messages, zoneData.state = ENDED) remain host-owned.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2870: BATTLE fail-by-death residual suite (AllNPCsDead + ShouldFailByDeath)
//   - 3073: ShouldFailByDeath (allNPCsDead || allPlayersDead)
//
// Production host (Lua BATTLE state) injects:
//   allNPCsDead    = #zoneData.npcs == zoneData.deadNPCCount  (AllNPCsDead)
//   allPlayersDead = not utils.any(players, isAliveFn)
// into ShouldFailByDeath before setting zoneData.state = ENDED.
// Go dual-wire: garrison.ShouldFailByDeath
// (internal/garrison/fail_by_death.go). Prior pure port: 2870;
// residual sibling: AllNPCsDead (npcCount == deadNPCCount).
//
// Parity: internal/garrison fail_by_death.go

namespace garrisonhelpers
{

// ---------------------------------------------------------------------------
// Slice 2870 — BATTLE state fail-by-death residual suite
// Slice 3073 — ShouldFailByDeath pure dual-wire expansion
// ---------------------------------------------------------------------------

// AllNPCsDead mirrors #zoneData.npcs == zoneData.deadNPCCount.
// Empty npc list with 0 dead → true (Lua equality).
// Residual pure helper (slice 2870); compose sibling of ShouldFailByDeath 3073.
inline auto AllNPCsDead(const int32 npcCount, const int32 deadNPCCount) -> bool
{
    return npcCount == deadNPCCount;
}

// ShouldFailByDeath reports whether garrison BATTLE ends by wipe.
//
// Formula (slice 3073 dual-wire):
//   allNPCsDead || allPlayersDead
//
// Host-injected scalars (no entity / zone / player pointers):
//   allNPCsDead    — host #npcs == deadNPCCount (see AllNPCsDead)
//   allPlayersDead — host not utils.any(players, isAlive)
//
// true  → host sets zoneData.state = ENDED (and prints fail message)
// false → leave BATTLE running
//
// Dual-wire of Go garrison.ShouldFailByDeath
// (internal/garrison/fail_by_death.go). Prior pure port: slice 2870.
// Residual sibling: AllNPCsDead (npcCount == deadNPCCount; host pre-resolution
// of the NPC wipe flag). Production host remains Lua until inject lands.
inline auto ShouldFailByDeath(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

} // namespace garrisonhelpers
