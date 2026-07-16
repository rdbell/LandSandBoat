#pragma once
namespace zoneentityvisibility { constexpr auto CharacterSyncDistance = 45.0f; constexpr auto ShouldConsiderPC(const float distance) -> bool { return distance <= CharacterSyncDistance; } }
