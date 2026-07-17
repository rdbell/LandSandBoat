#pragma once

#include "common/cbasetypes.h"

// Pure follow helpers shared by dual-wire slices:
//   - 2883: xi.follow.follow spawn+zone gate (CanFollow)
//
// Production host is Lua under scripts/globals/follow.lua
// (xi.follow.follow early return). Capacity is for future Lua/C++ inject so
// hosts dual-wire pure free functions instead of re-inlining the spawn/zone
// comparisons. Helpers take host-injected scalars only (no entity pointers).
// Side effects (entity follow/unfollow, local-var leaderID, DESPAWN listener)
// remain host-owned.
//
// Parity: internal/follow can_follow.go

namespace followhelpers
{

// ---------------------------------------------------------------------------
// Slice 2883 — xi.follow.follow spawn+zone eligibility gate
// ---------------------------------------------------------------------------

// CanFollow mirrors the pure half of xi.follow.follow's early return:
//
//   if not leader:isSpawned() or not follower:isSpawned()
//      or leader:getZoneID() ~= follower:getZoneID() then
//     return false
//   end
//
// Host injects isSpawned / getZoneID results. Host still owns Follow bookkeeping
// and entity follow()/setLocalVar after a true gate.
inline auto CanFollow(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

} // namespace followhelpers
