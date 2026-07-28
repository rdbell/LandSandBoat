#pragma once

// Pure CZone::CharZoneOut zone-wide treasure-pool reset gate.
namespace zonehelpers
{

inline auto ShouldResetZoneTreasurePool(const bool hasPool, const bool isZonePool, const bool charListEmpty) -> bool
{
    return hasPool && isZonePool && charListEmpty;
}

} // namespace zonehelpers
