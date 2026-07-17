#pragma once

#include "common/cbasetypes.h"

// Pure Garrison BATTLE fail-by-death gate helpers (slice 2870).
//
// Production host is Lua scripts/globals/garrison.lua BATTLE state
// (~420–434). Capacity is for future Lua/C++ inject so hosts dual-wire
// pure free functions instead of re-inlining comparisons. Helpers take
// host-injected scalars only (no entity / zone / player pointers).
// Side effects (messages, zoneData.state = ENDED) remain host-owned.
//
// Parity: internal/garrison fail_by_death.go

namespace garrisonhelpers
{

// ---------------------------------------------------------------------------
// Slice 2870 — BATTLE state fail-by-death gate
// ---------------------------------------------------------------------------

// AllNPCsDead mirrors #zoneData.npcs == zoneData.deadNPCCount.
// Empty npc list with 0 dead → true (Lua equality).
inline auto AllNPCsDead(const int32 npcCount, const int32 deadNPCCount) -> bool
{
    return npcCount == deadNPCCount;
}

// ShouldFailByDeath reports whether garrison BATTLE ends by wipe:
//   allNPCsDead or allPlayersDead  →  host sets zoneData.state = ENDED
// allNPCsDead: host #npcs == deadNPCCount (see AllNPCsDead)
// allPlayersDead: host not utils.any(players, isAlive)
inline auto ShouldFailByDeath(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

} // namespace garrisonhelpers
