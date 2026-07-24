#pragma once

#include <cstdint>

#include "entities/mob_entity.h"

namespace spawnconditionbased
{

inline auto requiresConditionEvent(const uint8_t spawnType) -> bool
{
    return spawnType & (SPAWNTYPE_ATNIGHT | SPAWNTYPE_ATEVENING | SPAWNTYPE_WEATHER | SPAWNTYPE_FOG);
}

} // namespace spawnconditionbased
