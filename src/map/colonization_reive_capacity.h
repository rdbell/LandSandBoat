#pragma once

#include "common/cbasetypes.h"

// Pure colonization reive helpers shared by dual-wire slices:
//   - 1038: residual pure port (internal/colonizationreive)
//   - 2882: ShouldSpawnOnEnable residual dual-wire suite (spawn_on_enable)
//   - 2889: ShouldDespawnOnDisable residual dual-wire suite (despawn_on_disable)
//   - 3163: ShouldSpawnOnEnable prior dedicated dual-wire (spawn_on_enable.go)
//   - 3189: ShouldDespawnOnDisable prior dedicated dual-wire (despawn_on_disable.go)
//   - 3252: ShouldDespawnOnDisable dedicated dual-wire expand residual 2889
//           (prior dedicated 3189; despawn_on_disable.go)
//   - 3335: ShouldSpawnOnEnable prior dedicated dual-wire expand residual 2882
//           (prior dedicated 3163; spawn_on_enable.go)
//   - 3411: ShouldSpawnOnEnable prior dedicated dual-wire expand residual 2882
//           (prior dedicated 3335 / prior dedicated 3163; spawn_on_enable.go)
//   - 3465: ShouldSpawnOnEnable prior dedicated dual-wire expand residual 2882
//           (prior dedicated 3411 / prior dedicated 3335 / prior dedicated 3163;
//            spawn_on_enable.go)
//   - 3499: ShouldSpawnOnEnable prior dedicated dual-wire expand residual 2882
//           (prior dedicated 3465 / prior dedicated 3411 / prior dedicated 3335 /
//            prior dedicated 3163; spawn_on_enable.go)
//   - 3593: ShouldSpawnOnEnable prior dedicated dual-wire expand residual 2882
//           (prior dedicated 3499 / prior dedicated 3465 / prior dedicated 3411 /
//            prior dedicated 3335 / prior dedicated 3163; spawn_on_enable.go)
//   - 3638: ShouldSpawnOnEnable prior dedicated dual-wire expand residual 2882
//           (prior dedicated 3593 / prior dedicated 3499 / prior dedicated 3465 /
//            prior dedicated 3411 / prior dedicated 3335 / prior dedicated 3163;
//            spawn_on_enable.go)
//   - 3683: ShouldSpawnOnEnable dedicated dual-wire expand residual 2882
//           (prior dedicated 3638 / prior dedicated 3593 / prior dedicated 3499 /
//            prior dedicated 3465 / prior dedicated 3411 / prior dedicated 3335 /
//            prior dedicated 3163; spawn_on_enable.go)
//
// Dual-wire index:
//   - 2882: ShouldSpawnOnEnable residual dual-wire suite
//   - 2889: ShouldDespawnOnDisable residual dual-wire suite
//   - 3163: ShouldSpawnOnEnable = !isAlive (prior dedicated)
//   - 3189: ShouldDespawnOnDisable = isSpawned (prior dedicated)
//   - 3252: ShouldDespawnOnDisable = isSpawned (dedicated expand residual 2889)
//   - 3335: ShouldSpawnOnEnable = !isAlive (prior dedicated expand residual 2882)
//   - 3411: ShouldSpawnOnEnable = !isAlive (prior dedicated expand residual 2882)
//   - 3465: ShouldSpawnOnEnable = !isAlive (prior dedicated expand residual 2882)
//   - 3499: ShouldSpawnOnEnable = !isAlive (prior dedicated expand residual 2882)
//   - 3593: ShouldSpawnOnEnable = !isAlive (prior dedicated expand residual 2882)
//   - 3638: ShouldSpawnOnEnable = !isAlive (prior dedicated expand residual 2882)
//   - 3683: ShouldSpawnOnEnable = !isAlive (dedicated expand residual 2882)
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
// Prior dedicated dual-wire suite: 3163 (test_colonizationreive_spawn_on_enable_3163).
// Prior dedicated dual-wire expand residual suite: 3335
//   (test_colonizationreive_spawn_on_enable_3335).
// Prior dedicated dual-wire expand residual suite: 3411
//   (test_colonizationreive_spawn_on_enable_3411).
// Prior dedicated dual-wire expand residual suite: 3465
//   (test_colonizationreive_spawn_on_enable_3465).
// Prior dedicated dual-wire expand residual suite: 3499
//   (test_colonizationreive_spawn_on_enable_3499).
// Prior dedicated dual-wire expand residual suite: 3593
//   (test_colonizationreive_spawn_on_enable_3593).
// Prior dedicated dual-wire expand residual suite: 3638
//   (test_colonizationreive_spawn_on_enable_3638).
// Dedicated dual-wire expand residual suite: 3683
//   (test_colonizationreive_spawn_on_enable_3683).
// Go dual-wire: colonizationreive.ShouldDespawnOnDisable
//   (internal/colonizationreive/despawn_on_disable.go).
// Residual dual-wire suite: 2889 (test_reive_despawn_on_disable_2889).
// Prior dedicated dual-wire suite: 3189 (test_colonizationreive_despawn_on_disable_3189).
// Dedicated dual-wire expand residual suite: 3252
//   (test_colonizationreive_despawn_on_disable_3252).
// Prior pure port: OmegaXI slice 1038 (internal/colonizationreive).

namespace reivehelpers
{

// ---------------------------------------------------------------------------
// Slice 3683 — enableReive spawn gate
// (dedicated dual-wire expand residual 2882 / prior dedicated 3638 /
// prior dedicated 3593 / prior dedicated 3499 / prior dedicated 3465 /
// prior dedicated 3411 / prior dedicated 3335 / prior dedicated 3163)
// ---------------------------------------------------------------------------

// ShouldSpawnOnEnable mirrors enableReive spawn gate for defenders and
// obstacles:
//
//   if not mob:isAlive() then SpawnMob(entryId) end
//
// Formula (slice 3683 dedicated dual-wire expand residual 2882 / prior 3638 /
// prior 3593 / prior 3499 / prior 3465 / prior 3411 / prior 3335 / prior 3163 /
// pure 1038 — formula unchanged):
//   ShouldSpawnOnEnable(isAlive) = !isAlive
//
// isAlive is the host-injected mob:isAlive() result. Host still owns
// GetMobByID, SpawnMob, and any post-spawn configuration.
// Dual-wire of Go colonizationreive.ShouldSpawnOnEnable.
// Call site: future Lua enableReive inject (defenders / obstacles).
// Prior pure port: slice 1038. Residual dual-wire suite: 2882 /
// test_reive_spawn_on_enable_2882. Prior dedicated dual-wire suite: 3163 /
// test_colonizationreive_spawn_on_enable_3163. Prior dedicated dual-wire
// expand residual suite: 3335 / test_colonizationreive_spawn_on_enable_3335.
// Prior dedicated dual-wire expand residual suite: 3411 /
// test_colonizationreive_spawn_on_enable_3411. Prior dedicated dual-wire
// expand residual suite: 3465 / test_colonizationreive_spawn_on_enable_3465.
// Prior dedicated dual-wire expand residual suite: 3499 /
// test_colonizationreive_spawn_on_enable_3499. Prior dedicated dual-wire
// expand residual suite: 3593 / test_colonizationreive_spawn_on_enable_3593.
// Prior dedicated dual-wire expand residual suite: 3638 /
// test_colonizationreive_spawn_on_enable_3638. Dedicated dual-wire expand
// residual suite is test_colonizationreive_spawn_on_enable_3683.
// Host still owns GetMobByID, SpawnMob, setRespawnTime, obstacle combat
// flags, and door animation.
// Future Lua host injects isAlive into this helper instead of re-inlining
// `not isAlive`.
// Sibling left alone under slices 3189 / 3252 (not re-expanded there).
inline auto ShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

// ---------------------------------------------------------------------------
// Slice 3252 — disableReive despawn gate
// (dedicated dual-wire expand residual 2889 / prior dedicated 3189)
// ---------------------------------------------------------------------------

// ShouldDespawnOnDisable mirrors disableReive despawn gate for defenders and
// obstacles:
//
//   if mob:isSpawned() then DespawnMob(entryId) end
//
// Formula (slice 3252 dedicated dual-wire expand residual 2889 / prior 3189 /
// pure 1038 — formula unchanged):
//   ShouldDespawnOnDisable(isSpawned) = isSpawned
//
// isSpawned is the host-injected mob:isSpawned() result. Host still owns
// GetMobByID, DespawnMob, and any post-despawn configuration.
// Dual-wire of Go colonizationreive.ShouldDespawnOnDisable.
// Call site: future Lua disableReive inject (defenders / obstacles).
// Prior pure port: slice 1038. Residual dual-wire suite: 2889 /
// test_reive_despawn_on_disable_2889. Prior dedicated dual-wire suite: 3189 /
// test_colonizationreive_despawn_on_disable_3189. Dedicated dual-wire expand
// residual suite is test_colonizationreive_despawn_on_disable_3252.
// Host still owns GetMobByID, DespawnMob, setRespawnTime, and door animation.
// Future Lua host injects isSpawned into this helper instead of re-inlining
// `isSpawned`.
// Sibling ShouldSpawnOnEnable left alone under 3683 / prior 3638 / prior 3593 /
// prior 3499 / prior 3465 / prior 3411 / prior 3335 / prior 3163 / residual 2882
// (not re-expanded here).
inline auto ShouldDespawnOnDisable(const bool isSpawned) -> bool
{
    return isSpawned;
}

} // namespace reivehelpers
