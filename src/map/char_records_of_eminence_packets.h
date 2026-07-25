#pragma once

#include "common/cbasetypes.h"

#include <array>

namespace eminencepackethelpers
{

enum class Action : uint8
{
    Unity,
    ActiveLog,
    TimedRecordMessage,
    CompletionLog,
};

struct Packet
{
    Action action;
    uint16  value;

    auto operator==(const Packet&) const -> bool = default;
};

struct Plan
{
    std::array<Packet, 7> packets{};
    uint8                 count{};
    bool                  clearTimedRecordNotification{};

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto BuildPlan(const bool roeEnabled, const bool notifyTimedRecord, const uint16 activeTimedRecord) -> Plan
{
    Plan plan{};
    plan.packets[plan.count++] = { Action::Unity, 0 };
    if (!roeEnabled)
    {
        return plan;
    }

    plan.packets[plan.count++] = { Action::ActiveLog, 0 };
    if (notifyTimedRecord)
    {
        plan.packets[plan.count++]        = { Action::TimedRecordMessage, activeTimedRecord };
        plan.clearTimedRecordNotification  = true;
    }
    for (uint16 completionPart = 0; completionPart < 4; ++completionPart)
    {
        plan.packets[plan.count++] = { Action::CompletionLog, completionPart };
    }
    return plan;
}

} // namespace eminencepackethelpers
