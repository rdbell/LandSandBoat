#include "test_map_networking_packet_append_2705.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingPacketAppend2705SelfTests() -> bool
{
    struct Case
    {
        std::size_t bufferSize;
        std::size_t packetSize;
        std::size_t maximumBufferSize;
        std::size_t appendedPackets;
        std::size_t packetBudget;
        bool        expected;
    };
    constexpr std::array cases{
        Case{ 28, 100, 2500, 0, 32, true },
        Case{ 2400, 100, 2500, 0, 32, false }, // exactly full is rejected
        Case{ 2399, 100, 2500, 0, 32, true },
        Case{ 28, 100, 2500, 32, 32, false },
        Case{ 28, 100, 2500, 33, 32, false },
    };
    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::CanAppendPacketToCompression(test.bufferSize, test.packetSize, test.maximumBufferSize, test.appendedPackets, test.packetBudget) != test.expected)
        {
            std::cerr << "map networking packet append 2705 self-test failed\n";
            return false;
        }
    }
    return true;
}
