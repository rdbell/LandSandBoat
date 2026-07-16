#include "test_zone_pc_despawn_gate_2706.h"
#include "map/zone_pc_despawn_gate.h"
#include <iostream>
auto runZonePCDespawnGate2706SelfTests() -> bool
{
    using zoneentityvisibility::ShouldDespawnSpawnedPC;
    const bool ok = !ShouldDespawnSpawnedPC(false, false, true, true) &&
                    ShouldDespawnSpawnedPC(false, true, true, true) &&
                    !ShouldDespawnSpawnedPC(true, true, true, true) &&
                    ShouldDespawnSpawnedPC(false, false, false, true) &&
                    ShouldDespawnSpawnedPC(false, false, true, false);
    if (!ok) std::cerr << "zone PC despawn gate 2706 self-test failed\n";
    return ok;
}
