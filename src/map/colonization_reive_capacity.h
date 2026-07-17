#pragma once

#include "common/cbasetypes.h"

// Pure colonization reive helpers shared by dual-wire slices:
//   - 1038: residual pure port (internal/colonizationreive ShouldSpawnOnEnable)
//   - 2882: ShouldSpawnOnEnable residual dual-wire suite (spawn_on_enable)
//   - 2889: ShouldDespawnOnDisable dual-wire (despawn_on_disable)
//   - 3163: ShouldSpawnOnEnable dedicated dual-wire (spawn_on_enable.go)
//
// Dual-wire index:
//   - 2882: ShouldSpawnOnEnable residual dual-wire suite
//   - 2889: ShouldDespawnOnDisable dual-wire
//   - 3163: ShouldSpawnOnEnable = !isAlive
//
// Production host is Lua under scripts/globals/colonization_reives.lua
// xi.reives.enableReive / disableReive for both defenders (reiveData.mob)
// and obstacles (reiveData.obstacles):
//
//   if not mob:isAlive() then
//     SpawnMob(entryId)  -- Spawn the reive defenders / obstacles
//   end
//   if mob:isSpawned() then
//     DespawnMob(entryId)  -- Despawn defender / obstacle mobs on reive end
//   end
//
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining `not isAlive` / `isSpawned`. Helpers take
// host-injected scalars only (no mob / zone / entity pointers).
// Side effects (GetMobByID, SpawnMob, DespawnMob, setRespawnTime,
// setAnimation, obstacle combat flags) remain host-owned.
// Go dual-wire: colonizationreive.ShouldSpawnOnEnable
//   (internal/colonizationreive/spawn_on_enable.go).
// Residual dual-wire suite: 2882 (test_reive_spawn_on_enable_2882).
// Dedicated dual-wire suite: 3163 (test_colonizationreive_spawn_on_enable_3163).
// Prior pure port: OmegaXI slice 1038 (internal/colonizationreive).

namespace reivehelpers
{

// ---------------------------------------------------------------------------
// Slice 3163 — enableReive spawn gate (dedicated expand residual 2882)
// ---------------------------------------------------------------------------

// ShouldSpawnOnEnable mirrors enableReive spawn gate for defenders and
// obstacles:
//
//   if not mob:isAlive() then SpawnMob(entryId) end
//
// Formula (slice 3163 dedicated dual-wire; residual expand 2882 / pure 1038 —
// formula unchanged):
//   ShouldSpawnOnEnable(isAlive) = !isAlive
//
// isAlive is the host-injected mob:isAlive() result. Host still owns
// GetMobByID, SpawnMob, and any post-spawn configuration.
// Dual-wire of Go colonizationreive.ShouldSpawnOnEnable.
// Call site: future Lua enableReive inject (defenders / obstacles).
// Prior pure port: slice 1038. Residual dual-wire suite: 2882 /
// test_reive_spawn_on_enable_2882. Dedicated dual-wire suite is
// test_colonizationreive_spawn_on_enable_3163. Host still owns GetMobByID,
// SpawnMob, setRespawnTime, obstacle combat flags, and door animation.
// Future Lua host injects isAlive into this helper instead of re-inlining
// `not isAlive`.
inline auto ShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

// ---------------------------------------------------------------------------
// Slice 2889 — disableReive despawn gate
// ---------------------------------------------------------------------------

// ShouldDespawnOnDisable mirrors disableReive despawn gate for defenders and
// obstacles:
//
//   if mob:isSpawned() then DespawnMob(entryId) end
//
// isSpawned is the host-injected mob:isSpawned() result. Host still owns
// GetMobByID, DespawnMob, and any post-despawn configuration.
// Sibling residual only under slice 3163 (not re-expanded).
inline auto ShouldDespawnOnDisable(const bool isSpawned) -> bool
{
    return isSpawned;
}

} // namespace reivehelpers
