#include "test_char_key_item_packets_6895.h"

#include "map/char_key_item_packets.h"

#include <iostream>

auto runCharKeyItemPackets6895SelfTests() -> bool
{
    const bool ok = keyitempackethelpers::BuildPlan() == std::array<uint8, 8>{ 0, 1, 2, 3, 4, 5, 6, 7 };
    if (!ok)
    {
        std::cerr << "key item packet 6895 self-test failed\n";
    }
    return ok;
}
