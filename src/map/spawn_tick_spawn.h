#pragma once

#include "spawn_capacity.h"

namespace spawntickspawn
{

inline auto shouldKeepPending(const bool canSpawnNow, const bool luaAllowsSpawn) -> bool
{
    return spawnhelpers::ShouldKeepPendingWhenCannotSpawn(canSpawnNow, luaAllowsSpawn);
}

} // namespace spawntickspawn
