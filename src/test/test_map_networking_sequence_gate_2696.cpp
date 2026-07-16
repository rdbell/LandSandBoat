#include "test_map_networking_sequence_gate_2696.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingSequenceGate2696SelfTests() -> bool
{
    struct Case
    {
        uint16 packetSequence;
        uint16 lastClientPacketSequence;
        uint16 datagramSequence;
        bool   expected;
    };
    constexpr std::array cases{
        Case{ 11, 10, 12, true },
        Case{ 10, 10, 12, false },
        Case{ 9, 10, 12, false },
        Case{ 13, 10, 12, false },
        // Native behavior is deliberately not wrap-aware.
        Case{ 0, 0xFFFF, 0, false },
    };
    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::ShouldDispatchIncomingSmallPacket(test.packetSequence, test.lastClientPacketSequence, test.datagramSequence) != test.expected)
        {
            std::cerr << "map networking sequence gate 2696 self-test failed\n";
            return false;
        }
    }
    return true;
}
