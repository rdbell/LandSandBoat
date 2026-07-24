#pragma once

#include "spawn_capacity.h"

namespace spawntickslot
{

inline auto shouldProcess(const bool respawnAtAfterThreshold) -> bool
{
    return spawnhelpers::IsRespawnDueWithinWindow(respawnAtAfterThreshold);
}

inline auto shouldRemoveAfterAttempt(const bool spawnSucceeded) -> bool
{
    return spawnSucceeded;
}

} // namespace spawntickslot
