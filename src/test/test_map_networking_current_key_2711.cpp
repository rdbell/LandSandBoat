#include "test_map_networking_current_key_2711.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingCurrentKey2711SelfTests() -> bool
{
    const bool ok = mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption(0) &&
                    !mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption(1) &&
                    !mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption(-1);
    if (!ok)
    {
        std::cerr << "map networking current key 2711 self-test failed\n";
    }
    return ok;
}
