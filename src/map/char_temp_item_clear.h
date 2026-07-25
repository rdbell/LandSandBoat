#pragma once

#include <array>
#include <cstdint>

namespace tempitemclearhelpers
{
constexpr std::uint8_t TempItemLocation = 3;

enum class Action : std::uint8_t
{
    DeletePersistedItems,
    ClearItemContainer,
};

struct Plan
{
    std::array<Action, 2> actions{};
    std::uint8_t           count{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves ClearTempItems' ordered persistence and container work.
constexpr auto MakePlan() -> Plan
{
    return {
        .actions = { Action::DeletePersistedItems, Action::ClearItemContainer },
        .count   = 2,
    };
}
} // namespace tempitemclearhelpers
