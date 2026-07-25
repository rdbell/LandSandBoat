#include "test_char_unity_ranking_packets_6900.h"

#include "map/char_unity_ranking_packets.h"

#include <array>
#include <iostream>

auto runCharUnityRankingPackets6900SelfTests() -> bool
{
    using unityrankingpackethelpers::Action;

    const auto plan = unityrankingpackethelpers::BuildPlan();
    bool       ok   = plan.count == 64;
    const auto checkPrefix = [&plan, &ok](const std::size_t start, const UNITY_RESULTSET resultSet)
    {
        ok = plan.packets[start] == unityrankingpackethelpers::Packet{ Action::Base, resultSet, 0, 0 } && ok;
        ok = plan.packets[start + 1] == unityrankingpackethelpers::Packet{ Action::Members, resultSet, 1, 0 } && ok;
        ok = plan.packets[start + 2] == unityrankingpackethelpers::Packet{ Action::Points, resultSet, 2, 0 } && ok;
        for (uint8 dataType = 0x03; dataType < 0x10; ++dataType)
        {
            ok = plan.packets[start + dataType] == unityrankingpackethelpers::Packet{ Action::Base, resultSet, dataType, 0 } && ok;
        }
    };

    checkPrefix(0, UNITY_RESULTSET::PreviousWeek);
    checkPrefix(32, UNITY_RESULTSET::CurrentWeek);
    for (uint8 dataType = 0x10; dataType < 0x20; ++dataType)
    {
        ok = plan.packets[dataType] == unityrankingpackethelpers::Packet{ Action::Data, UNITY_RESULTSET::PreviousWeek, dataType, 0x0008 } && ok;
    }

    constexpr std::array<uint16, 16> currentValues = { 0x2007, 0x2CC2, 0x6867, 0x6E6F, 0, 0x3605, 0x2007, 0x6C6C, 0x616E, 0x6767, 0, 0x2007, 0x2007, 0x0022, 0x0004, 0x2007 };
    for (uint8 offset = 0; offset < currentValues.size(); ++offset)
    {
        const auto dataType = static_cast<uint8>(0x10 + offset);
        const auto action   = dataType == 0x14 ? Action::Personal : Action::Data;
        ok                  = plan.packets[48 + offset] == unityrankingpackethelpers::Packet{ action, UNITY_RESULTSET::CurrentWeek, dataType, currentValues[offset] } && ok;
    }

    if (!ok)
    {
        std::cerr << "unity ranking packets 6900 self-test failed\n";
    }
    return ok;
}
