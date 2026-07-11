#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure CZone admission / level-restriction / counter policy for native tests.

namespace zonehelpers
{

// CharTargidHighThreshold is the exclusive upper bound for player targids
// (0x700). At or above this value, insert packets are ignored.
constexpr uint16 CharTargidHighThreshold = 0x700;

// FormatIncreaseZoneCounterWarning mirrors IncreaseZoneCounter ShowWarning text.
inline auto FormatIncreaseZoneCounterWarning() -> std::string
{
    return "CZone::IncreaseZoneCounter() - PChar is null, or Player zone or Treasure Pools is not null.";
}

// FormatInsertCharTargidHighError mirrors ShowError when targid >= 0x700.
// Note production uses a printf-style format with a broken specifier ("03hX");
// the pure helper pins the fixed prefix only.
inline auto FormatInsertCharTargidHighErrorPrefix() -> std::string
{
    return "CZone::InsertChar : targid is high (03hX), update packets will be ignored";
}

// ShouldRejectIncreaseZoneCounter mirrors
// PChar null || loc.zone != null || PTreasurePool != null.
inline auto ShouldRejectIncreaseZoneCounter(
    const bool charNull,
    const bool alreadyInZone,
    const bool hasTreasurePool) -> bool
{
    return charNull || alreadyInZone || hasTreasurePool;
}

// ShouldRejectHighCharTargid mirrors targid >= 0x700 after GetNewCharTargID.
inline auto ShouldRejectHighCharTargid(const uint16 targid) -> bool
{
    return targid >= CharTargidHighThreshold;
}

// ShouldCreateZoneTimers mirrors !zoneTimerToken && !CharListEmpty after insert.
inline auto ShouldCreateZoneTimers(const bool hasZoneTimerToken, const bool charListEmpty) -> bool
{
    return !hasZoneTimerToken && !charListEmpty;
}

// ShouldStampZoneEmptyTime mirrors CharListEmpty after DecreaseZoneCounter.
inline auto ShouldStampZoneEmptyTime(const bool charListEmpty) -> bool
{
    return charListEmpty;
}

// ShouldDespawnPCOnLeave mirrors !CharListEmpty after decrease (else branch).
inline auto ShouldDespawnPCOnLeave(const bool charListEmpty) -> bool
{
    return !charListEmpty;
}

// --- updateCharLevelRestriction ---

// ShouldSkipLevelRestrictionUpdate mirrors already has LevelRestriction with
// same power as zone restriction (or null status effect after Has check quirk).
// hasRestriction is HasStatusEffect(LevelRestriction).
// statusNull is GetStatusEffect returned nullptr after Has said true (defensive).
// powerMatches is statusEffect->GetPower() == m_levelRestriction.
inline auto ShouldSkipLevelRestrictionUpdate(
    const bool hasRestriction,
    const bool statusNull,
    const bool powerMatches) -> bool
{
    if (!hasRestriction)
    {
        return false;
    }
    return statusNull || powerMatches;
}

// ShouldDeleteExistingLevelRestriction mirrors has restriction and not skip.
inline auto ShouldDeleteExistingLevelRestriction(
    const bool hasRestriction,
    const bool shouldSkip) -> bool
{
    return hasRestriction && !shouldSkip;
}

// ShouldApplyZoneLevelRestriction mirrors m_levelRestriction != 0.
inline auto ShouldApplyZoneLevelRestriction(const uint8 zoneLevelRestriction) -> bool
{
    return zoneLevelRestriction != 0;
}

// --- SetWeather ---

// FormatInvalidWeatherWarning mirrors ShowWarningFmt for invalid weather enum.
inline auto FormatInvalidWeatherWarning(const uint16 weatherValue) -> std::string
{
    return fmt::format("Weather value ({}) invalid.", weatherValue);
}

// ShouldRejectInvalidWeather mirrors !magic_enum::enum_contains.
// isValidEnum is host-evaluated enum_contains result.
inline auto ShouldRejectInvalidWeather(const bool isValidEnum) -> bool
{
    return !isValidEnum;
}

// ShouldSkipSameWeather mirrors weather_.current() == weather.
inline auto ShouldSkipSameWeather(const bool alreadyCurrent) -> bool
{
    return alreadyCurrent;
}

// WeatherPacketOffsetMin/Max for xirand::GetRandomNumber(4, 28) half-open.
constexpr uint16 WeatherPacketOffsetMin = 4;
constexpr uint16 WeatherPacketOffsetMaxExclusive = 28;

// --- CheckMobsPathedBack ---

// IsMobAwayFromHome mirrors (alive && !fullyHealed) || pathingHome.
// isDead false means alive/spawned for this check.
inline auto IsMobAwayFromHome(const bool isDead, const bool isFullyHealed, const bool isPathingHome) -> bool
{
    return ((!isDead && !isFullyHealed) || isPathingHome);
}

// ShouldReportAllMobsHomeAndHealed mirrors empty list or no away mobs.
// anyAway is true if any mob matched IsMobAwayFromHome.
inline auto ShouldReportAllMobsHomeAndHealed(const bool hasMobList, const bool anyAway) -> bool
{
    if (!hasMobList)
    {
        return true;
    }
    return !anyAway;
}

// CanUseMisc mirrors (miscMask & misc) == misc.
inline auto CanUseMisc(const uint16 miscMask, const uint16 misc) -> bool
{
    return (miscMask & misc) == misc;
}

// WeatherCycleDays mirrors zone WeatherCycle constant used in UpdateWeather.
constexpr uint32 WeatherCycleDays = 2160;

// Weather chance bands from UpdateWeather.
constexpr uint8 WeatherChanceRareExclusive   = 15; // [0, 15)
constexpr uint8 WeatherChanceCommonExclusive = 50; // [15, 50)
// [50, 100) normal

// SelectWeatherBand: 0 rare, 1 common, 2 normal.
inline auto SelectWeatherBand(const uint8 weatherChance) -> uint8
{
    if (weatherChance < WeatherChanceRareExclusive)
    {
        return 0;
    }
    if (weatherChance < WeatherChanceCommonExclusive)
    {
        return 1;
    }
    return 2;
}

// ShouldForceMorningFog mirrors fog window + non-elemental + non-city.
// inFogWindow is [02:00, 07:00) Vana'diel; selectedBelowHotSpell is
// selectedWeather < Weather::HotSpell; isCity is ZONE_TYPE::CITY mask.
inline auto ShouldForceMorningFog(
    const bool inFogWindow,
    const bool selectedBelowHotSpell,
    const bool isCity) -> bool
{
    return inFogWindow && selectedBelowHotSpell && !isCity;
}

// ShouldRescheduleDynamicWeather mirrors !weather().isStatic() on timer wake.
inline auto ShouldRescheduleDynamicWeather(const bool isStatic) -> bool
{
    return !isStatic;
}

// WeatherUpdateDelayMin/MaxExclusive for random 180..1801 seconds.
constexpr int WeatherUpdateDelayMinSeconds          = 180;
constexpr int WeatherUpdateDelayMaxExclusiveSeconds = 1801;

} // namespace zonehelpers
