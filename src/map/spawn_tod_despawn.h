#pragma once

#include <chrono>

#include "common/vana_time.h"
#include "spawn_capacity.h"

namespace spawntoddespawn
{

enum class Trigger : uint8
{
    None,
    NewDay,
    Dawn,
};

constexpr auto triggerFor(const vanadiel_time::TOTD totd) -> Trigger
{
    switch (totd)
    {
        case vanadiel_time::TOTD::NEWDAY:
            return Trigger::NewDay;
        case vanadiel_time::TOTD::DAWN:
            return Trigger::Dawn;
        default:
            return Trigger::None;
    }
}

inline auto shouldDespawn(const Trigger trigger, const uint8 spawnType) -> bool
{
    switch (trigger)
    {
        case Trigger::NewDay:
            return spawnhelpers::ShouldDespawnOnNewDay(spawnType);
        case Trigger::Dawn:
            return spawnhelpers::ShouldDespawnOnDawn(spawnType);
        case Trigger::None:
            return false;
    }

    return false;
}

constexpr auto delay() -> std::chrono::milliseconds
{
    return std::chrono::milliseconds{ 1 };
}

} // namespace spawntoddespawn
