#pragma once

namespace zoneentityvisibility
{

// ShouldDespawnSpawnedPC mirrors SpawnPCs' pre-distance removal gate.
constexpr auto ShouldDespawnSpawnedPC(const bool sameCharacter, const bool targetGMHidden, const bool sameMogHouse, const bool withinVerticalRange) -> bool
{
    return (!sameCharacter && targetGMHidden) || !sameMogHouse || !withinVerticalRange;
}

} // namespace zoneentityvisibility
