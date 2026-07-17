#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure jailutils::InPrison policy helpers (slice 2837).
//
// Production host: jailutils::InPrison in jailutils.cpp.
// Helpers take host-injected scalars only (no CCharEntity pointers).

namespace jailutilshelpers
{

// InPrisonPure is the fully scalar boolean form of InPrison:
//   gmLevelIsZero && zoneIsMordion
// Host injects the two comparison results (gmLevel == 0, zone == Mordion Gaol).
inline auto InPrisonPure(const bool gmLevelIsZero, const bool zoneIsMordion) -> bool
{
    return gmLevelIsZero && zoneIsMordion;
}

// InPrison mirrors jailutils::InPrison pure half:
//   gmLevel == 0 && zoneID == mordionGaolZoneID
// Host injects m_GMlevel, getZone(), and ZONE_MORDION_GAOL (no entity/zone headers).
inline auto InPrison(const uint8 gmLevel, const uint16 zoneID, const uint16 mordionGaolZoneID) -> bool
{
    return InPrisonPure(gmLevel == 0, zoneID == mordionGaolZoneID);
}

} // namespace jailutilshelpers
