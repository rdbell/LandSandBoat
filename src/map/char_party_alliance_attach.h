#pragma once

#include <array>
#include <cstdint>

namespace partyallianceattachhelpers
{
enum class Action : std::uint8_t
{
    CreateAlliance,
    AttachParty,
};

struct Plan
{
    std::array<Action, 2> actions{};
    std::uint8_t           count{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves ReloadParty's missing-local-alliance actions after lookup.
constexpr auto MakePlan(
    const bool hasPersistedAlliance,
    const bool hasLocalAlliance,
    const bool foundExistingAlliance) -> Plan
{
    if (!hasPersistedAlliance || hasLocalAlliance)
    {
        return {};
    }
    if (foundExistingAlliance)
    {
        return {
            .actions = { Action::AttachParty },
            .count   = 1,
        };
    }
    return {
        .actions = { Action::CreateAlliance, Action::AttachParty },
        .count   = 2,
    };
}
} // namespace partyallianceattachhelpers
