#include "test_map_networking_acknowledgement_2699.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingAcknowledgement2699SelfTests() -> bool
{
    using mapnetworkinghelpers::AcknowledgementPlan;
    struct Case
    {
        uint16              acknowledged;
        uint16              server;
        uint16              lastPacketType;
        AcknowledgementPlan expected;
    };
    constexpr std::array cases{
        Case{ 7, 7, 0x001, AcknowledgementPlan::IncrementServerPacketID },
        Case{ 6, 7, 0x00A, AcknowledgementPlan::IgnoreLoginMismatch },
        Case{ 6, 7, 0x001, AcknowledgementPlan::ReplayCachedPacket },
        // An acknowledgement match takes precedence over the login packet type.
        Case{ 7, 7, 0x00A, AcknowledgementPlan::IncrementServerPacketID },
    };
    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::PlanOutgoingPacketAcknowledgement(test.acknowledged, test.server, test.lastPacketType) != test.expected)
        {
            std::cerr << "map networking acknowledgement 2699 self-test failed\n";
            return false;
        }
    }
    return true;
}
