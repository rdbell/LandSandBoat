#pragma once

// Pure CZone::CharZoneIn treasure-pool assignment choice.
namespace zonehelpers
{

enum class ZoneInTreasurePoolAction
{
    AttachZonePool,
    ReloadPartyPool,
    CreateSoloPool,
};

inline auto PlanZoneInTreasurePool(const bool hasZonePool, const bool hasParty) -> ZoneInTreasurePoolAction
{
    if (hasZonePool)
    {
        return ZoneInTreasurePoolAction::AttachZonePool;
    }

    return hasParty ? ZoneInTreasurePoolAction::ReloadPartyPool : ZoneInTreasurePoolAction::CreateSoloPool;
}

} // namespace zonehelpers
