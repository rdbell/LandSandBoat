#include "test_map_networking_char_zone_2729.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingCharZone2729SelfTests() -> bool
{
    const bool ok = mapnetworkinghelpers::ShouldSendCharZoneAfterKeyIncrement(false) &&
                    !mapnetworkinghelpers::ShouldSendCharZoneAfterKeyIncrement(true);
    if (!ok)
    {
        std::cerr << "map networking char zone 2729 self-test failed\n";
    }
    return ok;
}
