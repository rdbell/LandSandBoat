#include "test_map_networking_last_update_2714.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingLastUpdate2714SelfTests() -> bool
{
    const bool ok = mapnetworkinghelpers::ShouldTapLastUpdate(false, false) &&
                    !mapnetworkinghelpers::ShouldTapLastUpdate(true, false) &&
                    !mapnetworkinghelpers::ShouldTapLastUpdate(false, true) &&
                    !mapnetworkinghelpers::ShouldTapLastUpdate(true, true);
    if (!ok)
    {
        std::cerr << "map networking last update 2714 self-test failed\n";
    }
    return ok;
}
