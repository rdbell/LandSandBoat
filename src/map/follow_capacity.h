#pragma once

#include "common/cbasetypes.h"

// Pure follow helpers shared by dual-wire slices:
//   - 2883: xi.follow.follow spawn+zone gate (CanFollow) — residual pure dual-wire
//   - 3084: CanFollow pure free-function dual-wire re-assert
//           (followerSpawned && leaderSpawned && followerZone == leaderZone)
//
// Production host is Lua under scripts/globals/follow.lua
// (xi.follow.follow early return). Capacity is for future Lua/C++ inject so
// hosts dual-wire pure free functions instead of re-inlining the spawn/zone
// comparisons. Helpers take host-injected scalars only (no entity pointers).
// Side effects (entity follow/unfollow, local-var leaderID, DESPAWN listener)
// remain host-owned.
//
// Parity: internal/follow can_follow.go
// Coverage: test_follow_can_follow_2883 (residual), test_follow_can_follow_3084
// (dual-wire; not in CMake/main).

namespace followhelpers
{

// ---------------------------------------------------------------------------
// Slice 2883 residual / 3084 dual-wire — xi.follow.follow spawn+zone gate
// ---------------------------------------------------------------------------

// CanFollow mirrors the pure half of xi.follow.follow's early return:
//
//   if not leader:isSpawned() or not follower:isSpawned()
//      or leader:getZoneID() ~= follower:getZoneID() then
//     return false
//   end
//
// Formula (slice 3084 dual-wire; residual 2883):
//   followerSpawned && leaderSpawned && followerZone == leaderZone
//
// Host injects isSpawned / getZoneID results. Host still owns Follow bookkeeping
// and entity follow()/setLocalVar after a true gate.
// Dual-wire of Go follow.CanFollow (internal/follow/can_follow.go).
// Coverage: test_follow_can_follow_3084 (not in CMake/main).
inline auto CanFollow(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

} // namespace followhelpers
