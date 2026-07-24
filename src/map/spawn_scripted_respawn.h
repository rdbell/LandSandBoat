#pragma once

#include <cstdint>

#include "entities/mob_entity.h"

namespace spawnscriptedrespawn
{

inline auto shouldAllow(const uint8_t spawnType, const bool hasRespawnDelay) -> bool
{
    return spawnType == SPAWNTYPE_SCRIPTED && hasRespawnDelay;
}

} // namespace spawnscriptedrespawn
