#pragma once

#include "common/cbasetypes.h"

// Pure follow helpers shared by dual-wire slices:
//   - 2883: xi.follow.follow spawn+zone gate (CanFollow) — residual pure dual-wire
//   - 3084: CanFollow prior dedicated pure free-function dual-wire
//           (followerSpawned && leaderSpawned && followerZone == leaderZone)
//   - 3237: CanFollow prior dedicated dual-wire expand residual 2883
//           (prior dedicated 3084; formula unchanged)
//   - 3331: CanFollow dedicated dual-wire expand residual 2883
//           (prior dedicated 3084 / 3237; formula unchanged)
//
// Dual-wire index:
//   - 2883: CanFollow residual pure dual-wire
//   - 3084: CanFollow prior dedicated dual-wire
//   - 3237: CanFollow prior dedicated expand residual 2883
//   - 3331: CanFollow =
//       followerSpawned && leaderSpawned && followerZone == leaderZone
//     dedicated dual-wire expand residual 2883 (prior dedicated 3084 / 3237)
//
// Production host is Lua under scripts/globals/follow.lua
// (xi.follow.follow early return). Capacity is for future Lua/C++ inject so
// hosts dual-wire pure free functions instead of re-inlining the spawn/zone
// comparisons. Helpers take host-injected scalars only (no entity pointers).
// Side effects (entity follow/unfollow, local-var leaderID, DESPAWN listener)
// remain host-owned.
//
// Parity: internal/follow can_follow.go
// Coverage: test_follow_can_follow_2883 (residual),
// test_follow_can_follow_3084 (prior dedicated dual-wire; not in CMake/main),
// test_follow_can_follow_3237 (prior dedicated expand residual 2883; not in CMake/main),
// test_follow_can_follow_3331 (dedicated expand residual 2883; not in CMake/main).

namespace followhelpers
{

// ---------------------------------------------------------------------------
// Slice 2883 residual / 3084 prior dedicated / 3237 prior expand /
// 3331 expand residual 2883 — xi.follow.follow spawn+zone gate
// ---------------------------------------------------------------------------

// CanFollow mirrors the pure half of xi.follow.follow's early return:
//
//   if not leader:isSpawned() or not follower:isSpawned()
//      or leader:getZoneID() ~= follower:getZoneID() then
//     return false
//   end
//
// Formula (slice 3331 dual-wire expand residual 2883; prior dedicated 3084 /
// 3237):
//   followerSpawned && leaderSpawned && followerZone == leaderZone
//
// Host injects isSpawned / getZoneID results. Host still owns Follow bookkeeping
// and entity follow()/setLocalVar after a true gate.
// Dual-wire of Go follow.CanFollow (internal/follow/can_follow.go).
// Coverage: test_follow_can_follow_3331 (not in CMake/main); residual 2883 /
// prior dedicated 3084 / 3237 suites retained.
inline auto CanFollow(const bool followerSpawned, const bool leaderSpawned, const uint16 followerZone, const uint16 leaderZone) -> bool
{
    return followerSpawned && leaderSpawned && followerZone == leaderZone;
}

} // namespace followhelpers
