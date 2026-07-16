#include "test_map_networking_key_increment_2726.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingKeyIncrement2726SelfTests() -> bool
{
    const bool ok = !mapnetworkinghelpers::ShouldIncrementKeyAfterEncrypt(0x000) &&
                    !mapnetworkinghelpers::ShouldIncrementKeyAfterEncrypt(0x00A) &&
                    mapnetworkinghelpers::ShouldIncrementKeyAfterEncrypt(0x00B) &&
                    !mapnetworkinghelpers::ShouldIncrementKeyAfterEncrypt(0x00C);
    if (!ok)
    {
        std::cerr << "map networking key increment 2726 self-test failed\n";
    }
    return ok;
}
