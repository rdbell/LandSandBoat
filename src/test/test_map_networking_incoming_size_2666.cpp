#include "test_map_networking_incoming_size_2666.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingIncomingSize2666SelfTests() -> bool
{
    constexpr auto minimumPacketSize = FFXI_HEADER_SIZE + 1 + sizeof(uint32) + 16;
    struct case_t
    {
        std::size_t packetSize;
        std::size_t outputCapacity;
        bool        expected;
    };

    constexpr auto cases = std::array{
        case_t{ minimumPacketSize - 1, 100, false },
        case_t{ minimumPacketSize, 100, true },
        case_t{ 100, 100, true },
        case_t{ 101, 100, false },
    };

    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::HasValidIncomingPacketSize(test.packetSize, test.outputCapacity) != test.expected)
        {
            std::cerr << "map networking incoming size 2666 self-test failed\n";
            return false;
        }
    }

    // Both production lengths are std::size_t; preserve unsigned conversion
    // for a synthetic negative output capacity that wraps above the frame.
    if (!mapnetworkinghelpers::HasValidIncomingPacketSize(minimumPacketSize, -1))
    {
        std::cerr << "map networking incoming size signed wrap self-test failed\n";
        return false;
    }
    return true;
}
