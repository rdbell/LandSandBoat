#pragma once

#include "spawn_capacity.h"

namespace spawncanspawn
{

inline auto canSpawnNow(const bool                    mobNull,
                        const bool                    allowRespawn,
                        const uint8                   spawnType,
                        const vanadiel_time::TOTD     totd,
                        const bool                    isFog,
                        const bool                    isElemental,
                        const bool                    hasMaster,
                        const bool                    elementMatchesWeather) -> bool
{
    return spawnhelpers::CanSpawnNowPure(
        mobNull, allowRespawn, spawnType, totd, isFog, isElemental, hasMaster, elementMatchesWeather);
}

} // namespace spawncanspawn
