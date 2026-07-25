#pragma once

#include <array>
#include <cstdint>

namespace partyreloadmissinghelpers
{
enum class Action : std::uint8_t
{
    RemoveMember,
    DecrementReload,
};

struct Plan
{
    std::array<Action, 2> actions{};
    std::uint8_t           count{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves ReloadParty's ordered actions when its query has no row.
constexpr auto MakePlan(const bool hasParty) -> Plan
{
    if (hasParty)
    {
        return {
            .actions = { Action::RemoveMember, Action::DecrementReload },
            .count   = 2,
        };
    }
    return {
        .actions = { Action::DecrementReload },
        .count   = 1,
    };
}
} // namespace partyreloadmissinghelpers
