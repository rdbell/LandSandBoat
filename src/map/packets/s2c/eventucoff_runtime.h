#pragma once

#include "0x052_eventucoff.h"

namespace eventucoffhelpers
{

struct Facts
{
    bool  hasCurrentEvent{};
    int32 eventID{};
};

struct Plan
{
    uint32 mode{};
    bool   resetSubstate{ true };
};

[[nodiscard]] constexpr auto PlanFor(const GP_SERV_COMMAND_EVENTUCOFF_MODE mode, const Facts& facts) -> Plan
{
    auto value = static_cast<uint32>(mode);
    if (mode == GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelEvent && facts.hasCurrentEvent)
    {
        value |= static_cast<uint32>(facts.eventID) << 8;
    }

    return { .mode = value };
}

} // namespace eventucoffhelpers
