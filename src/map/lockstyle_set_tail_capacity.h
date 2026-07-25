#pragma once

#include <array>
#include <cstdint>

// Pure Set prelude and post-style tail from GP_CLI_COMMAND_LOCKSTYLE::process.

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
    bool                      setStyleLock = false;
    std::array<ActionKind, 3> actions{};
};

constexpr auto PlanFor() -> Plan
{
    return {
        .setStyleLock = true,
        .actions = {
            ActionKind::UpdateRemovedSlots,
            ActionKind::Persist,
            ActionKind::Refresh,
        },
    };
}

} // namespace lockstylesettailhelpers
