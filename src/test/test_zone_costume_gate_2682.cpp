#include "test_zone_costume_gate_2682.h"
#include "map/zone_capacity.h"

auto runZoneCostumeGate2682SelfTests() -> bool
{
    return zonehelpers::ShouldClearCostumeOnZoneIn(true) && !zonehelpers::ShouldClearCostumeOnZoneIn(false);
}
