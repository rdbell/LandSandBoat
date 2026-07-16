#include "test_zone_pc_spawn_gate_2703.h"

#include "map/zone_pc_spawn_gate.h"

#include <iostream>

auto runZonePCSpawnGate2703SelfTests() -> bool
{
    const bool ordinary = zoneentityvisibility::ShouldSpawnPCs(100);
    const bool garden   = zoneentityvisibility::ShouldSpawnPCs(zoneentityvisibility::MogGardenZone);
    const bool feretory = zoneentityvisibility::ShouldSpawnPCs(zoneentityvisibility::FeretoryZone);
    if (!ordinary || garden || feretory)
    {
        std::cerr << "zone PC spawn gate 2703 self-test failed\n";
        return false;
    }
    return true;
}
