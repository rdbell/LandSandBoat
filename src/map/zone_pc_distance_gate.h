#pragma once
namespace zoneentityvisibility
{
constexpr auto CharacterDespawnDistance = 50.0f;
constexpr auto ShouldDespawnDistantPC(const float distance) -> bool { return distance >= CharacterDespawnDistance; }
} // namespace zoneentityvisibility
