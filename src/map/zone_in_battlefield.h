#pragma once

// Pure CZone::CharZoneIn battlefield/confrontation reconciliation.
namespace zonehelpers
{

enum class ZoneInBattlefieldAction
{
    None,
    InsertRegistered,
    AddOrphaned,
    ClearConfrontation,
    ClearLevelSync,
};

struct ZoneInBattlefieldPlan
{
    ZoneInBattlefieldAction action{ ZoneInBattlefieldAction::None };
    bool                    entered{};
    bool                    clearPetConfrontation{};
    bool                    updateLevelRestriction{};

    auto operator==(const ZoneInBattlefieldPlan&) const -> bool = default;
};

// PlanZoneInBattlefield mirrors CharZoneIn after mount/costume/treasure setup.
// A present handler suppresses the otherwise-later no-party LevelSync cleanup.
inline auto PlanZoneInBattlefield(const bool hasHandler, const bool hasRegisteredBattlefield,
                                  const bool hasConfrontation, const bool playerEntered,
                                  const bool hasPet, const bool hasLevelSync, const bool hasParty) -> ZoneInBattlefieldPlan
{
    if (hasHandler)
    {
        if (hasRegisteredBattlefield && hasConfrontation)
        {
            return { ZoneInBattlefieldAction::InsertRegistered, playerEntered, false, false };
        }
        if (hasConfrontation)
        {
            if (playerEntered)
            {
                return { ZoneInBattlefieldAction::AddOrphaned, false, false, false };
            }
            return { ZoneInBattlefieldAction::ClearConfrontation, false, hasPet, true };
        }
        return {};
    }

    if (hasConfrontation)
    {
        return { ZoneInBattlefieldAction::ClearConfrontation, false, hasPet, false };
    }
    if (hasLevelSync && !hasParty)
    {
        return { ZoneInBattlefieldAction::ClearLevelSync, false, false, false };
    }
    return {};
}

} // namespace zonehelpers
