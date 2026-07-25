#pragma once

#include <array>
#include <cstdint>

// Pure post-style tail from GP_CLI_COMMAND_LOCKSTYLE::process Set mode.

namespace lockstylesettailhelpers
{

enum class ActionKind : std::uint8_t
{
    UpdateRemovedSlots,
    Persist,
    Refresh,
};

struct Plan
{
    std::array<ActionKind, 3> actions{};
};

constexpr auto PlanFor() -> Plan
{
    return {
        .actions = {
            ActionKind::UpdateRemovedSlots,
            ActionKind::Persist,
            ActionKind::Refresh,
        },
    };
}

} // namespace lockstylesettailhelpers
