#pragma once

#include <chrono>

#include "spawn_capacity.h"

namespace spawnweatherdespawn
{

inline auto shouldDespawn(const bool  isElemental,
                          const bool  hasMaster,
                          const uint8 spawnType,
                          const bool  elementMatchesWeather,
                          const bool  isFog) -> bool
{
    if (spawnhelpers::ShouldDespawnElementalOnWeather(isElemental, hasMaster, spawnType, elementMatchesWeather))
    {
        return true;
    }

    return spawnhelpers::ShouldDespawnFogMobOnWeather(spawnType, isFog);
}

constexpr auto delay() -> std::chrono::seconds
{
    return std::chrono::seconds{ 1 };
}

} // namespace spawnweatherdespawn
