#pragma once

#include "packets/s2c/0x063_miscdata_unity.h"

#include <array>

namespace unityrankingpackethelpers
{

enum class Action : uint8
{
    Base,
    Members,
    Points,
    Data,
    Personal,
};

struct Packet
{
    Action          action;
    UNITY_RESULTSET resultSet;
    uint8           dataType;
    uint16          value;

    auto operator==(const Packet&) const -> bool = default;
};

struct Plan
{
    std::array<Packet, 64> packets{};
    uint8                  count{};

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto BuildPlan() -> Plan
{
    auto plan = Plan{};
    const auto add = [&plan](const Action action, const UNITY_RESULTSET resultSet, const uint8 dataType, const uint16 value = 0)
    {
        plan.packets[plan.count++] = { action, resultSet, dataType, value };
    };
    const auto addPrefix = [&add](const UNITY_RESULTSET resultSet)
    {
        add(Action::Base, resultSet, static_cast<uint8>(UNITY_DATATYPE::Base));
        add(Action::Members, resultSet, static_cast<uint8>(UNITY_DATATYPE::Members));
        add(Action::Points, resultSet, static_cast<uint8>(UNITY_DATATYPE::Points));
        for (uint8 dataType = 0x03; dataType < 0x10; ++dataType)
        {
            add(Action::Base, resultSet, dataType);
        }
    };

    addPrefix(UNITY_RESULTSET::PreviousWeek);
    for (uint8 dataType = 0x10; dataType < 0x20; ++dataType)
    {
        add(Action::Data, UNITY_RESULTSET::PreviousWeek, dataType, 0x0008);
    }

    addPrefix(UNITY_RESULTSET::CurrentWeek);
    constexpr std::array<uint16, 16> currentValues = { 0x2007, 0x2CC2, 0x6867, 0x6E6F, 0, 0x3605, 0x2007, 0x6C6C, 0x616E, 0x6767, 0, 0x2007, 0x2007, 0x0022, 0x0004, 0x2007 };
    for (uint8 offset = 0; offset < currentValues.size(); ++offset)
    {
        const auto dataType = static_cast<uint8>(0x10 + offset);
        add(dataType == 0x14 ? Action::Personal : Action::Data, UNITY_RESULTSET::CurrentWeek, dataType, currentValues[offset]);
    }

    return plan;
}

} // namespace unityrankingpackethelpers
