#pragma once

#include "common/cbasetypes.h"

namespace timerpackethelpers
{
enum class Action : uint8
{
    SendTimer,
    ClearTimer,
};

struct Plan
{
    Action action;
    uint32  seconds;

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto BuildTimerPlan(uint32 seconds) -> Plan
{
    return { Action::SendTimer, seconds };
}

constexpr auto BuildClearPlan() -> Plan
{
    return { Action::ClearTimer, 0 };
}
} // namespace timerpackethelpers
