#include "test_map_networking_incoming_blocks_2663.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingIncomingBlocks2663SelfTests() -> bool
{
    struct case_t
    {
        std::size_t packetSize;
        std::size_t expected;
    };

    constexpr auto cases = std::array{
        case_t{ FFXI_HEADER_SIZE - 1, 0 },
        case_t{ FFXI_HEADER_SIZE, 0 },
        case_t{ FFXI_HEADER_SIZE + 7, 0 },
        case_t{ FFXI_HEADER_SIZE + 8, 1 },
        case_t{ FFXI_HEADER_SIZE + 15, 1 },
        case_t{ FFXI_HEADER_SIZE + 16, 2 },
    };

    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::IncomingEncryptedBlockCount(test.packetSize) != test.expected)
        {
            std::cerr << "map networking incoming blocks 2663 self-test failed\n";
            return false;
        }
    }
    return true;
}
