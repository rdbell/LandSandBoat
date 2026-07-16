#include "test_zone_mount_gate_2673.h"
#include "map/zone_capacity.h"

auto runZoneMountGate2673SelfTests() -> bool
{
    return zonehelpers::ShouldDismountOnZoneIn(true, false) && !zonehelpers::ShouldDismountOnZoneIn(true, true) && !zonehelpers::ShouldDismountOnZoneIn(false, false);
}
