#pragma once

#include <cstdint>

// Pure shared CCharEntity/CZone level-sync departure handling.
namespace partyhelpers
{

enum class LevelSyncDepartureAction
{
    None,
    DeactivateLeftArea,
    RemoveTooFewMembers,
};

struct LevelSyncDeparturePlan
{
    LevelSyncDepartureAction action{ LevelSyncDepartureAction::None };
    bool                     clearDepartingEffects{};

    auto operator==(const LevelSyncDeparturePlan&) const -> bool = default;
};

// PlanLevelSyncDeparture mirrors the restricted-player departure path. A
// departing designee or leader deactivates first, so the too-few-members path
// cannot also run. Same-zonePeerCount excludes the departing player.
inline auto PlanLevelSyncDeparture(const bool hasLevelRestriction, const bool hasParty,
                                   const bool departingIsSyncTargetOrLeader, const bool hasSyncTarget,
                                   const std::uint8_t sameZonePeerCount) -> LevelSyncDeparturePlan
{
    if (!hasLevelRestriction)
    {
        return {};
    }
    if (!hasParty)
    {
        return { LevelSyncDepartureAction::None, true };
    }
    if (departingIsSyncTargetOrLeader)
    {
        return { LevelSyncDepartureAction::DeactivateLeftArea, true };
    }
    if (hasSyncTarget && sameZonePeerCount < 2)
    {
        return { LevelSyncDepartureAction::RemoveTooFewMembers, true };
    }
    return { LevelSyncDepartureAction::None, true };
}

} // namespace partyhelpers
