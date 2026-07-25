#pragma once

namespace charvarclearallhelpers
{
struct Plan
{
    bool deletePersisted{};
    bool refreshLocalCaches{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves ClearCharVarFromAll's local-only behavior.
constexpr auto MakePlan(const bool localOnly) -> Plan
{
    return {
        .deletePersisted     = !localOnly,
        .refreshLocalCaches = true,
    };
}
} // namespace charvarclearallhelpers
