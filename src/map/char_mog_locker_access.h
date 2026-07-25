#pragma once

#include <cstdint>

namespace moglockeraccesshelpers
{
constexpr std::uint32_t MogLockerAccessAlZahbi   = 0;
constexpr std::uint32_t MogLockerAccessAllAreas  = 1;
constexpr std::uint16_t ZoneAlZahbi              = 48;
constexpr std::uint16_t ZoneAhtUrhganWhitegate   = 50;
constexpr std::uint16_t ZoneNashmau              = 53;

// HasMogLockerAccess mirrors hasMogLockerAccess after character and zone facts
// are injected.
constexpr auto HasMogLockerAccess(
    const std::uint32_t now,
    const std::uint32_t expiry,
    const std::uint32_t accessType,
    const std::uint16_t zoneID,
    const bool zoneAllowsMogMenu,
    const bool ownMogHouse) -> bool
{
    if (now >= expiry)
    {
        return false;
    }
    if (accessType == MogLockerAccessAllAreas)
    {
        return zoneAllowsMogMenu || ownMogHouse;
    }
    return (ownMogHouse && (zoneID == ZoneAlZahbi || zoneID == ZoneAhtUrhganWhitegate)) || zoneID == ZoneNashmau;
}
} // namespace moglockeraccesshelpers
