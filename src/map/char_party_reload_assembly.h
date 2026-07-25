#pragma once

#include <array>
#include <cstdint>

namespace partyreloadassemblyhelpers
{
enum class Action : std::uint8_t
{
    CreateParty,
    PushMember,
};

struct Plan
{
    std::array<Action, 2> actions{};
    std::uint8_t           count{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves ReloadParty's missing-local-party actions after lookup.
constexpr auto MakePlan(const bool hasLocalParty, const bool foundExistingParty) -> Plan
{
    if (hasLocalParty)
    {
        return {};
    }
    if (foundExistingParty)
    {
        return {
            .actions = { Action::PushMember },
            .count   = 1,
        };
    }
    return {
        .actions = { Action::CreateParty, Action::PushMember },
        .count   = 2,
    };
}
} // namespace partyreloadassemblyhelpers
