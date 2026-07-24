#pragma once

namespace zoneentityvisibility
{

// ShouldDispatchPCDespawn mirrors CZoneEntities::DespawnPC's SpawnPCList
// membership gate before it erases the entry and emits ENTITY_DESPAWN.
constexpr auto ShouldDispatchPCDespawn(const bool isInSpawnList) -> bool
{
    return isInSpawnList;
}

} // namespace zoneentityvisibility
