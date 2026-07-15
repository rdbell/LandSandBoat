#pragma once

#include <cstdint>

// Pure CBattleEntity zone-location predicates: isInDynamis, isInAssault,
// isInAdoulin. Host retains zone pointer resolution; helpers take only
// hasZone flags and zone properties.
// Parity: internal/battlezone (slice 1643).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::isInDynamis  (~178–186)
//   CBattleEntity::isInAssault  (~188–196)
//   CBattleEntity::isInAdoulin  (~198–216)

namespace battlezonehelpers
{

// ZONE_TYPE bit pins (zone.h).
constexpr std::uint16_t ZoneTypeDynamis   = 0x0080; // ZONE_TYPE::DYNAMIS
constexpr std::uint16_t ZoneTypeInstanced = 0x0100; // ZONE_TYPE::INSTANCED

// REGION_TYPE inclusive assault bounds (zone.h).
constexpr std::uint8_t RegionWestAhtUrhgan = 28; // REGION_TYPE::WEST_AHT_URHGAN
constexpr std::uint8_t RegionAlzadaal      = 32; // REGION_TYPE::ALZADAAL

// Adoulin-area ZONEID pins (zone.h).
constexpr std::uint16_t ZoneWesternAdoulin          = 256; // ZONE_WESTERN_ADOULIN
constexpr std::uint16_t ZoneEasternAdoulin          = 257; // ZONE_EASTERN_ADOULIN
constexpr std::uint16_t ZoneMogGarden               = 280; // ZONE_MOG_GARDEN
constexpr std::uint16_t ZoneSilverKnife             = 283; // ZONE_SILVER_KNIFE
constexpr std::uint16_t ZoneCelenniaMemorialLibrary = 284; // ZONE_CELENNIA_MEMORIAL_LIBRARY

// IsInDynamis mirrors CBattleEntity::isInDynamis after zone resolution.
// hasZone is false when both loc.zone and GetZone(loc.destination) are null.
// Otherwise returns (typeMask & ZONE_TYPE::DYNAMIS) != 0.
inline auto IsInDynamis(const bool hasZone, const std::uint16_t typeMask) -> bool
{
    if (!hasZone)
    {
        return false;
    }
    return (typeMask & ZoneTypeDynamis) != 0;
}

// IsInAssault mirrors CBattleEntity::isInAssault.
// hasZone false (loc.zone == nullptr) → false.
// Else: INSTANCED mask set AND regionID in [WEST_AHT_URHGAN, ALZADAAL].
// Unlike isInDynamis, does not fall back to loc.destination.
inline auto IsInAssault(const bool hasZone, const std::uint16_t typeMask, const std::uint8_t regionID) -> bool
{
    if (!hasZone)
    {
        return false;
    }
    return (typeMask & ZoneTypeInstanced) != 0 &&
           (regionID >= RegionWestAhtUrhgan && regionID <= RegionAlzadaal);
}

// IsInAdoulin mirrors CBattleEntity::isInAdoulin.
// hasZone false (loc.zone == nullptr) → false.
// Else true when zoneID is one of the five Adoulin-area zones.
inline auto IsInAdoulin(const bool hasZone, const std::uint16_t zoneID) -> bool
{
    if (!hasZone)
    {
        return false;
    }
    switch (zoneID)
    {
        case ZoneWesternAdoulin:
        case ZoneEasternAdoulin:
        case ZoneMogGarden:
        case ZoneSilverKnife:
        case ZoneCelenniaMemorialLibrary:
            return true;
        default:
            return false;
    }
}

} // namespace battlezonehelpers
