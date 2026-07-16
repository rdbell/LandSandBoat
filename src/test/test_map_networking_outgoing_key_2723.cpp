#include "test_map_networking_outgoing_key_2723.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingOutgoingKey2723SelfTests() -> bool
{
    const bool ok = !mapnetworkinghelpers::ShouldUsePreviousKeyForOutgoingPacket(false, false) &&
                    !mapnetworkinghelpers::ShouldUsePreviousKeyForOutgoingPacket(false, true) &&
                    !mapnetworkinghelpers::ShouldUsePreviousKeyForOutgoingPacket(true, false) &&
                    mapnetworkinghelpers::ShouldUsePreviousKeyForOutgoingPacket(true, true);
    if (!ok)
    {
        std::cerr << "map networking outgoing key 2723 self-test failed\n";
    }
    return ok;
}
