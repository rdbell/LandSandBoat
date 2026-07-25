#pragma once

#include <array>
#include <cstdint>

namespace partylevelsyncrestorehelpers
{
enum class Action : std::uint8_t
{
    SendActivation,
    RemoveDispelableEffects,
    AddLevelSync,
};

struct Plan
{
    std::array<Action, 3> actions{};
    std::uint8_t           count{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves ReloadParty's level-sync restoration gate and ordered
// actions.
constexpr auto MakePlan(
    const bool hasSyncTarget,
    const bool inSameZone,
    const bool characterHasLevelSync,
    const bool targetHasLevelSync,
    const bool targetLevelSyncInfinite) -> Plan
{
    if (!hasSyncTarget || !inSameZone || characterHasLevelSync || !targetHasLevelSync || !targetLevelSyncInfinite)
    {
        return {};
    }
    return {
        .actions = { Action::SendActivation, Action::RemoveDispelableEffects, Action::AddLevelSync },
        .count   = 3,
    };
}
} // namespace partylevelsyncrestorehelpers
