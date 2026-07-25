#pragma once

#include <cstdint>

namespace partyalliancereconcilehelpers
{
enum class Action : std::uint8_t
{
    None,
    SynchronizeID,
    DetachParty,
};

struct Plan
{
    Action action{ Action::None };
};

// MakePlan preserves ReloadParty's existing-local-alliance reconciliation.
constexpr auto MakePlan(
    const std::uint32_t persistedAllianceID,
    const bool          hasLocalAlliance,
    const std::uint32_t localAllianceID) -> Plan
{
    if (persistedAllianceID != 0)
    {
        if (hasLocalAlliance && localAllianceID != persistedAllianceID)
        {
            return { .action = Action::SynchronizeID };
        }
        return {};
    }
    if (hasLocalAlliance)
    {
        return { .action = Action::DetachParty };
    }
    return {};
}
} // namespace partyalliancereconcilehelpers
