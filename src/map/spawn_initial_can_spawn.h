#pragma once

#include <cstdint>

#include "entities/mob_entity.h"

namespace spawninitialcanspawn
{

inline auto canSpawn(const uint8_t spawnType) -> bool
{
    return spawnType == SPAWNTYPE_NORMAL ||
           spawnType == SPAWNTYPE_LOTTERY ||
           spawnType == SPAWNTYPE_SCRIPTED ||
           spawnType == SPAWNTYPE_WINDOWED;
}

} // namespace spawninitialcanspawn
