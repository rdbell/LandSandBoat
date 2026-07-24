#include "test_zone_pc_despawn_dispatch_6253.h"

#include "map/zone_pc_despawn_dispatch.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone PC despawn dispatch 6253 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins CZoneEntities::DespawnPC's recipient SpawnPCList gate before removal
// and ENTITY_DESPAWN dispatch.
auto runZonePCDespawnDispatch6253SelfTests() -> bool
{
    bool ok = true;
    for (const bool isInSpawnList : { false, true })
    {
        ok = expect(zoneentityvisibility::ShouldDispatchPCDespawn(isInSpawnList) == isInSpawnList,
                    "PC despawn requires source in recipient SpawnPCList") &&
             ok;
    }
    return ok;
}
