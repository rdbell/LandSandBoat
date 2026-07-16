#pragma once

#include <cstdint>

namespace zoneentityvisibility
{

constexpr std::uint16_t MogGardenZone = 280;
constexpr std::uint16_t FeretoryZone  = 285;

// ShouldSpawnPCs mirrors SpawnPCs' explicit solo-zone exception.
constexpr auto ShouldSpawnPCs(const std::uint16_t zoneID) -> bool
{
    return zoneID != FeretoryZone && zoneID != MogGardenZone;
}

} // namespace zoneentityvisibility
