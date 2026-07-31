#include "test_map_networking_login_size_2687.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingLoginSize2687SelfTests() -> bool
{
    const bool ok = mapnetworkinghelpers::HasUnencryptedLoginPacketSize(FFXI_HEADER_SIZE + 20, 20) &&
                    !mapnetworkinghelpers::HasUnencryptedLoginPacketSize(FFXI_HEADER_SIZE + 19, 20) &&
                    mapnetworkinghelpers::HasUnencryptedLoginPacketSize(FFXI_HEADER_SIZE, 0) &&
                    !mapnetworkinghelpers::HasUnencryptedLoginPacketSize(FFXI_HEADER_SIZE - 1, 0) &&
                    mapnetworkinghelpers::HasUnencryptedLoginPacketSize(-1, 20);
    if (!ok)
    {
        std::cerr << "map networking login size 2687 self-test failed\n";
    }
    return ok;
}
