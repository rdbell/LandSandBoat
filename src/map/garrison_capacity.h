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
//   - 3073: ShouldFailByDeath prior dedicated dual-wire
//   - 3236: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3330: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3406: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3461: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3514: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3560: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3605: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3650: ShouldFailByDeath prior dedicated dual-wire expand residual 2870
//   - 3695: ShouldFailByDeath dedicated dual-wire expand residual 2870
//
// Production host (Lua BATTLE state) injects:
//   allNPCsDead    = #zoneData.npcs == zoneData.deadNPCCount  (AllNPCsDead)
//   allPlayersDead = not utils.any(players, isAliveFn)
// into ShouldFailByDeath before setting zoneData.state = ENDED.
// Go dual-wire: garrison.ShouldFailByDeath
// (internal/garrison/fail_by_death.go). Prior pure port: 2870;
// prior dedicated dual-wire: 3073;
// prior dedicated expand residual: 3236;
// prior dedicated expand residual: 3330;
// prior dedicated expand residual: 3406;
// prior dedicated expand residual: 3461;
// prior dedicated expand residual: 3514;
// prior dedicated expand residual: 3560;
// prior dedicated expand residual: 3605;
// prior dedicated expand residual: 3650;
// dedicated dual-wire suite: 3695 (expand residual 2870).
// Residual sibling: AllNPCsDead (npcCount == deadNPCCount).
//
// Parity: internal/garrison fail_by_death.go

namespace garrisonhelpers
{

// ---------------------------------------------------------------------------
// Slice 2870 — BATTLE state fail-by-death residual suite
// Slice 3073 — ShouldFailByDeath prior pure dual-wire expansion
// Slice 3236 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3330 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3406 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3461 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3514 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3560 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3605 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3650 — ShouldFailByDeath prior dedicated dual-wire expand residual 2870
// Slice 3695 — ShouldFailByDeath dedicated dual-wire expand residual 2870
// ---------------------------------------------------------------------------

// AllNPCsDead mirrors #zoneData.npcs == zoneData.deadNPCCount.
// Empty npc list with 0 dead → true (Lua equality).
// Residual pure helper (slice 2870); compose sibling of ShouldFailByDeath
// (3073 prior dedicated / 3236 prior dedicated expand / 3330 prior dedicated expand /
// 3406 prior dedicated expand / 3461 prior dedicated expand /
// 3514 prior dedicated expand / 3560 prior dedicated expand /
// 3605 prior dedicated expand / 3650 prior dedicated expand /
// 3695 dedicated expand residual).
inline auto AllNPCsDead(const int32 npcCount, const int32 deadNPCCount) -> bool
{
    return npcCount == deadNPCCount;
}

// ShouldFailByDeath reports whether garrison BATTLE ends by wipe.
//
// Formula (slice 3695 dedicated dual-wire; residual expand 2870 / prior
// dedicated 3073 / prior dedicated expand 3236 / prior dedicated expand 3330 /
// prior dedicated expand 3406 / prior dedicated expand 3461 /
// prior dedicated expand 3514 / prior dedicated expand 3560 /
// prior dedicated expand 3605 / prior dedicated expand 3650 — formula unchanged):
//   allNPCsDead || allPlayersDead
//
// Pin form: direct return of the positive OR only (no !(a && b) De Morgan;
// QF1001).
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
// Prior dedicated dual-wire: slice 3073 / test_garrison_fail_by_death_3073.
// Prior dedicated expand residual: slice 3236 / test_garrison_fail_by_death_3236.
// Prior dedicated expand residual: slice 3330 / test_garrison_fail_by_death_3330.
// Prior dedicated expand residual: slice 3406 / test_garrison_fail_by_death_3406.
// Prior dedicated expand residual: slice 3461 / test_garrison_fail_by_death_3461.
// Prior dedicated expand residual: slice 3514 / test_garrison_fail_by_death_3514.
// Prior dedicated expand residual: slice 3560 / test_garrison_fail_by_death_3560.
// Prior dedicated expand residual: slice 3605 / test_garrison_fail_by_death_3605.
// Prior dedicated expand residual: slice 3650 / test_garrison_fail_by_death_3650.
// Dedicated dual-wire suite: slice 3695 / test_garrison_fail_by_death_3695.
// Residual sibling: AllNPCsDead (npcCount == deadNPCCount; host pre-resolution
// of the NPC wipe flag). Production host remains Lua until inject lands.
inline auto ShouldFailByDeath(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

} // namespace garrisonhelpers
