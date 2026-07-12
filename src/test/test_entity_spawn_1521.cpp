#include "test_entity_spawn_1521.h"

#include "map/entity_spawn_capacity.h"

#include <iostream>

namespace
{
using entityspawnhelpers::IsRaceChangeArgsInBounds;
using entityspawnhelpers::IsRaceChangeEquipSlotInRange;
using entityspawnhelpers::IsSpawnedInList;
using entityspawnhelpers::ShouldRejectHomePointAtCap;
using entityspawnhelpers::ShouldRejectNullCharRaceChange;
using entityspawnhelpers::ShouldRejectNullEntity;
using entityspawnhelpers::ShouldResetHPMPOnHomePoint;
using entityspawnhelpers::ShouldUnequipOnRaceChange;
using entityspawnhelpers::SpawnListKind;
using entityspawnhelpers::SpawnListKindFromObjType;

auto Check() -> bool
{
    if (!ShouldRejectNullEntity(false) || ShouldRejectNullEntity(true))
    {
        return false;
    }
    if (SpawnListKindFromObjType(0x04, 0x04, 0x02, 0x01, 0x08, 0x20) != SpawnListKind::Mob ||
        SpawnListKindFromObjType(0x02, 0x04, 0x02, 0x01, 0x08, 0x20) != SpawnListKind::NPC ||
        SpawnListKindFromObjType(0x01, 0x04, 0x02, 0x01, 0x08, 0x20) != SpawnListKind::PC ||
        SpawnListKindFromObjType(0x08, 0x04, 0x02, 0x01, 0x08, 0x20) != SpawnListKind::Pet ||
        SpawnListKindFromObjType(0x20, 0x04, 0x02, 0x01, 0x08, 0x20) != SpawnListKind::Trust ||
        SpawnListKindFromObjType(0x10, 0x04, 0x02, 0x01, 0x08, 0x20) != SpawnListKind::None)
    {
        return false;
    }
    if (!IsSpawnedInList(true) || IsSpawnedInList(false))
    {
        return false;
    }
    if (!ShouldRejectNullCharRaceChange(false) || ShouldRejectNullCharRaceChange(true))
    {
        return false;
    }
    if (!IsRaceChangeArgsInBounds(1, 0, 0, 1, 8, 15, 2) || !IsRaceChangeArgsInBounds(8, 15, 2, 1, 8, 15, 2) ||
        IsRaceChangeArgsInBounds(0, 0, 0, 1, 8, 15, 2) || IsRaceChangeArgsInBounds(9, 0, 0, 1, 8, 15, 2) ||
        IsRaceChangeArgsInBounds(1, 16, 0, 1, 8, 15, 2) || IsRaceChangeArgsInBounds(1, 0, 3, 1, 8, 15, 2))
    {
        return false;
    }
    if (!ShouldUnequipOnRaceChange(false) || ShouldUnequipOnRaceChange(true))
    {
        return false;
    }
    if (!IsRaceChangeEquipSlotInRange(0, 0, 15) || !IsRaceChangeEquipSlotInRange(15, 0, 15) || IsRaceChangeEquipSlotInRange(16, 0, 15))
    {
        return false;
    }
    if (!ShouldRejectHomePointAtCap(true) || ShouldRejectHomePointAtCap(false))
    {
        return false;
    }
    if (!ShouldResetHPMPOnHomePoint(true) || ShouldResetHPMPOnHomePoint(false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runEntitySpawn1521SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "entity spawn 1521 self-test failed\n";
    }
    return ok;
}
