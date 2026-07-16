#include "test_map_networking_checksum_2690.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingChecksum2690SelfTests() -> bool
{
    const bool ok = mapnetworkinghelpers::IsChecksumValid(0) && !mapnetworkinghelpers::IsChecksumValid(-1) && !mapnetworkinghelpers::IsChecksumValid(1);
    if (!ok)
    {
        std::cerr << "map networking checksum 2690 self-test failed\n";
    }
    return ok;
}
