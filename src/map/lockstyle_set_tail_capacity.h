#pragma once

// Pure post-style tail from GP_CLI_COMMAND_LOCKSTYLE::process Set mode.

namespace lockstylesettailhelpers
{

struct Plan
{
    bool updateRemovedSlots = false;
    bool persist            = false;
    bool refresh            = false;
};

constexpr auto PlanFor() -> Plan
{
    return {
        .updateRemovedSlots = true,
        .persist            = true,
        .refresh            = true,
    };
}

} // namespace lockstylesettailhelpers
