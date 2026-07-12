#pragma once

#include <cstdint>
#include <tuple>
#include <utility>

// Pure mobutils::GetBaseWeaponDamage offset planning.
// Parity: internal/mobutils base_weapon_damage.go (slice 1601).
// Host still owns setMobMod writeback and returns level as setDamage.

namespace mobweapondamagehelpers
{

// REGION_TYPE pins (zone.h)
constexpr std::uint8_t RegionRonfaure        = 0;
constexpr std::uint8_t RegionZulkheim        = 1;
constexpr std::uint8_t RegionNorvallen       = 2;
constexpr std::uint8_t RegionGustaberg       = 3;
constexpr std::uint8_t RegionDerfland        = 4;
constexpr std::uint8_t RegionSarutabaruta    = 5;
constexpr std::uint8_t RegionKolshushu       = 6;
constexpr std::uint8_t RegionAragoneu        = 7;
constexpr std::uint8_t RegionFauregandi      = 8;
constexpr std::uint8_t RegionValdeaunia      = 9;
constexpr std::uint8_t RegionQufimIsland     = 10;
constexpr std::uint8_t RegionLitelor         = 11;
constexpr std::uint8_t RegionKuzotz          = 12;
constexpr std::uint8_t RegionVollbow         = 13;
constexpr std::uint8_t RegionElshimoLowlands = 14;
constexpr std::uint8_t RegionElshimoUplands  = 15;
constexpr std::uint8_t RegionTulia           = 16;
constexpr std::uint8_t RegionMovalpolos      = 17;
constexpr std::uint8_t RegionTavnazia        = 18;
constexpr std::uint8_t RegionSandoria        = 19;
constexpr std::uint8_t RegionBastok          = 20;
constexpr std::uint8_t RegionWindurst        = 21;
constexpr std::uint8_t RegionJeuno           = 22;
constexpr std::uint8_t RegionDynamis         = 23;
constexpr std::uint8_t RegionTavnazianMarq   = 24;
constexpr std::uint8_t RegionPromyvion       = 25;
constexpr std::uint8_t RegionLumoria         = 26;
constexpr std::uint8_t RegionLimbus          = 27;
constexpr std::uint8_t RegionWestAhtUrhgan   = 28;
constexpr std::uint8_t RegionMamoolJaSavage  = 29;
constexpr std::uint8_t RegionHalvung         = 30;
constexpr std::uint8_t RegionArrapago        = 31;
constexpr std::uint8_t RegionAlzadaal        = 32;
constexpr std::uint8_t RegionRonfaureFront   = 33;
constexpr std::uint8_t RegionNorvallenFront  = 34;
constexpr std::uint8_t RegionGustabergFront  = 35;
constexpr std::uint8_t RegionDerflandFront   = 36;
constexpr std::uint8_t RegionSarutaFront     = 37;
constexpr std::uint8_t RegionAragoneauFront  = 38;
constexpr std::uint8_t RegionFauregandiFront = 39;
constexpr std::uint8_t RegionValdeauniaFront = 40;
constexpr std::uint8_t RegionAbyssea         = 41;
constexpr std::uint8_t RegionTheThreshold    = 42;
constexpr std::uint8_t RegionAbdhaljs        = 43;
constexpr std::uint8_t RegionAdoulinIslands  = 44;
constexpr std::uint8_t RegionEastUlbuka      = 45;

// Beginner zones that lower offsets by 1 for non-NMs.
constexpr std::uint16_t ZoneWestRonfaure     = 100;
constexpr std::uint16_t ZoneEastRonfaure     = 101;
constexpr std::uint16_t ZoneNorthGustaberg   = 106;
constexpr std::uint16_t ZoneSouthGustaberg   = 107;
constexpr std::uint16_t ZoneWestSarutabaruta = 115;
constexpr std::uint16_t ZoneEastSarutabaruta = 116;

constexpr auto IsBeginnerDamageZone(const std::uint16_t zoneId) -> bool
{
    switch (zoneId)
    {
        case ZoneWestRonfaure:
        case ZoneEastRonfaure:
        case ZoneNorthGustaberg:
        case ZoneSouthGustaberg:
        case ZoneWestSarutabaruta:
        case ZoneEastSarutabaruta:
            return true;
        default:
            return false;
    }
}

// Returns {meleeOffset, rangedOffset} for region before beginner adjust.
constexpr auto RegionOffsets(const std::uint8_t regionId) -> std::pair<std::int8_t, std::int8_t>
{
    switch (regionId)
    {
        // Vanilla / ROTZ / COP
        case RegionRonfaure:
        case RegionZulkheim:
        case RegionNorvallen:
        case RegionGustaberg:
        case RegionDerfland:
        case RegionSarutabaruta:
        case RegionKolshushu:
        case RegionAragoneu:
        case RegionFauregandi:
        case RegionValdeaunia:
        case RegionQufimIsland:
        case RegionLitelor:
        case RegionKuzotz:
        case RegionVollbow:
        case RegionElshimoLowlands:
        case RegionElshimoUplands:
        case RegionTulia:
        case RegionMovalpolos:
        case RegionTavnazia:
        case RegionSandoria:
        case RegionBastok:
        case RegionWindurst:
        case RegionJeuno:
        case RegionDynamis:
        case RegionTavnazianMarq:
        case RegionPromyvion:
        case RegionLumoria:
        case RegionLimbus:
            return { 2, 5 };
        // TOAU
        case RegionWestAhtUrhgan:
        case RegionMamoolJaSavage:
        case RegionHalvung:
        case RegionArrapago:
        case RegionAlzadaal:
            return { 10, 12 };
        // WOTG fronts
        case RegionRonfaureFront:
        case RegionNorvallenFront:
        case RegionGustabergFront:
        case RegionDerflandFront:
        case RegionSarutaFront:
        case RegionAragoneauFront:
        case RegionFauregandiFront:
        case RegionValdeauniaFront:
            return { 11, 13 };
        // Other
        case RegionAbyssea:
        case RegionTheThreshold:
        case RegionAbdhaljs:
            return { 11, 13 };
        // SOA
        case RegionAdoulinIslands:
        case RegionEastUlbuka:
            return { 11, 13 };
        default:
            return { 2, 5 };
    }
}

// Beginner-zone −1 for non-NMs, then clamp ≥ 0.
constexpr auto ApplyBeginnerZoneOffsetAdjust(std::int8_t offset, std::int8_t rangedOffset, const std::uint16_t zoneId, const bool isNM)
    -> std::pair<std::int8_t, std::int8_t>
{
    if (zoneId != 0 && !isNM && IsBeginnerDamageZone(zoneId))
    {
        offset -= 1;
        rangedOffset -= 1;
    }
    if (offset < 0)
    {
        offset = 0;
    }
    if (rangedOffset < 0)
    {
        rangedOffset = 0;
    }
    return { offset, rangedOffset };
}

// Full pure plan: setDamage = level; MOBMOD offsets after region + beginner.
constexpr auto PlanBaseWeaponDamage(const std::uint8_t regionId, const std::uint16_t level, const std::uint16_t zoneId, const bool isNM)
    -> std::tuple<std::uint16_t, std::int8_t, std::int8_t>
{
    const auto offsets = RegionOffsets(regionId);
    const auto adjusted = ApplyBeginnerZoneOffsetAdjust(offsets.first, offsets.second, zoneId, isNM);
    return { level, adjusted.first, adjusted.second };
}

} // namespace mobweapondamagehelpers
