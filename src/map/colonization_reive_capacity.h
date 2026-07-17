#pragma once

#include "common/cbasetypes.h"

// Pure colonization reive helpers shared by dual-wire slices:
//   - 2882: enableReive spawn gate (ShouldSpawnOnEnable)
//
// Production host is Lua under scripts/globals/colonization_reives.lua
// xi.reives.enableReive for both defenders (reiveData.mob) and obstacles
// (reiveData.obstacles):
//
//   if not mob:isAlive() then
//     SpawnMob(entryId)  -- Spawn the reive defenders / obstacles
//   end
//
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining `not isAlive`. Helpers take
// host-injected scalars only (no mob / zone / entity pointers).
// Side effects (GetMobByID, SpawnMob, setRespawnTime, setAnimation,
// obstacle combat flags) remain host-owned.
// Prior pure port: OmegaXI slice 1038 (internal/colonizationreive).

namespace reivehelpers
{

// ---------------------------------------------------------------------------
// Slice 2882 — enableReive spawn gate
// ---------------------------------------------------------------------------

// ShouldSpawnOnEnable mirrors enableReive spawn gate for defenders and
// obstacles:
//
//   if not mob:isAlive() then SpawnMob(entryId) end
//
// isAlive is the host-injected mob:isAlive() result. Host still owns
// GetMobByID, SpawnMob, and any post-spawn configuration.
inline auto ShouldSpawnOnEnable(const bool isAlive) -> bool
{
    return !isAlive;
}

} // namespace reivehelpers
