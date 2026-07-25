#pragma once

#include "common/cbasetypes.h"

#include <array>

namespace dropitemdispatchhelpers
{

enum class Action : uint8
{
    Log,
    ThrowAwayMessage,
    ItemSame,
};

struct Plan
{
    std::array<Action, 3> actions{};
    uint8                 count{};

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto BuildPlan(const bool itemUpdated) -> Plan
{
    if (!itemUpdated)
    {
        return {};
    }
    return { .actions = { Action::Log, Action::ThrowAwayMessage, Action::ItemSame }, .count = 3 };
}

} // namespace dropitemdispatchhelpers
