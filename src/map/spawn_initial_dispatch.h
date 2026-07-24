#pragma once

namespace spawninitialdispatch
{

inline auto shouldSpawn(const bool canSpawn, const bool allowRespawn) -> bool
{
    return canSpawn && allowRespawn;
}

} // namespace spawninitialdispatch
