#include "test_zone_pc_distance_gate_2709.h"
#include "map/zone_pc_distance_gate.h"
auto runZonePCDistanceGate2709SelfTests() -> bool { return !zoneentityvisibility::ShouldDespawnDistantPC(49.999f) && zoneentityvisibility::ShouldDespawnDistantPC(50.0f); }
