#pragma once

#include "spawn_capacity.h"

namespace spawntickcleanup
{

inline auto shouldProcess(const bool respawnAtAfterThreshold) -> bool
{
    return spawnhelpers::IsRespawnDueWithinWindow(respawnAtAfterThreshold);
}

inline auto shouldDrop(const bool mobFound) -> bool
{
    return spawnhelpers::ShouldDropMissingMobRegistration(mobFound);
}

} // namespace spawntickcleanup
