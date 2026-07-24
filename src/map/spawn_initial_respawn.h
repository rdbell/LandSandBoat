#pragma once

#include <cstdint>

#include "entities/mob_entity.h"

namespace spawninitialrespawn
{

inline auto allowsRespawn(const uint8_t spawnType) -> bool
{
    return spawnType != SPAWNTYPE_LOTTERY &&
           spawnType != SPAWNTYPE_SCRIPTED &&
           spawnType != SPAWNTYPE_WINDOWED;
}

} // namespace spawninitialrespawn
