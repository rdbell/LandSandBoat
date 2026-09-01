#include "test_zone_residential_area_8961.h"

#include "map/zone_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

auto runZoneResidentialArea8961SelfTests() -> bool
{
    const auto ok = zonehelpers::IsResidentialArea(true) && !zonehelpers::IsResidentialArea(false);
    if (!ok)
    {
        std::cerr << "zone residential area 8961 self-test failed\n";
    }
    return ok;
}

OMEGA_REGISTER_SELF_TEST("zone-residential-area-8961", runZoneResidentialArea8961SelfTests);
