#pragma once

#include "common/cbasetypes.h"
#include "common/timer.h"

#include "map_constants.h"

#include <chrono>
#include <cstdint>
#include <fmt/format.h>
#include <optional>
#include <set>
#include <string>

// Pure CZone admission / level-restriction / counter policy for native tests.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1363: zone admission / level-restriction / weather / counter policy suite
//   - 2652: ShouldStopZoneTimers (idle shutdown)
//   - 2848: IsMobAwayFromHome / ShouldReportAllMobsHomeAndHealed
//   - 2939: ShouldStampZoneEmptyTime (charListEmpty after DecreaseZoneCounter)
//   - 2949: ShouldRejectHighCharTargid (targid >= CharTargidHighThreshold / 0x700)
//   - 2975: ShouldDespawnPCOnLeave (!charListEmpty after DecreaseZoneCounter)
//   - 2992: ShouldCreateZoneTimers (!hasZoneTimerToken && !charListEmpty after InsertPC)
//   - 3019: ShouldRejectInvalidWeather (!isValidEnum / !enum_contains on SetWeather)
//   - 3028: ShouldSkipSameWeather (alreadyCurrent on SetWeather)
//   - 3032: ShouldApplyZoneLevelRestriction (zoneLevelRestriction != 0 on
//           updateCharLevelRestriction)
//
// Production host: CZone::DecreaseZoneCounter (zone.cpp) injects
// CharListEmpty() into ShouldStampZoneEmptyTime; on true stamps m_timeZoneEmpty;
// else if ShouldDespawnPCOnLeave calls DespawnPC.
// Go dual-wire: zone.ShouldStampZoneEmptyTime (internal/zone/stamp_empty.go);
// zone.ShouldDespawnPCOnLeave (internal/zone/despawn_pc_leave.go).
// Production host: CZone::IncreaseZoneCounter (zone.cpp) injects
// GetNewCharTargID() into ShouldRejectHighCharTargid; on true ShowError + return.
// Go dual-wire: zone.ShouldRejectHighCharTargid (internal/zone/high_targid.go).
// Production host: CZone::IncreaseZoneCounter (zone.cpp) injects
// zoneTimerToken_.has_value() and CharListEmpty() into ShouldCreateZoneTimers
// after InsertPC; on true calls createZoneTimers().
// Go dual-wire: zone.ShouldCreateZoneTimers (internal/zone/create_zone_timers.go).
// Production host: CZone::SetWeather (zone.cpp) injects
// magic_enum::enum_contains<Weather>(weather) into ShouldRejectInvalidWeather;
// on true ShowWarningFmt(FormatInvalidWeatherWarning) + return.
// Go dual-wire: zone.ShouldRejectInvalidWeather
// (internal/zone/reject_invalid_weather.go).
// Production host: CZone::SetWeather (zone.cpp) injects
// weather_.current() == weather into ShouldSkipSameWeather; on true return.
// Go dual-wire: zone.ShouldSkipSameWeather (internal/zone/skip_same_weather.go).
// Production host: CZone::updateCharLevelRestriction (zone.cpp) injects
// m_levelRestriction into ShouldApplyZoneLevelRestriction; on true strips
// buffs + AddStatusEffect LevelRestriction.
// Go dual-wire: zone.ShouldApplyZoneLevelRestriction
// (internal/zone/apply_level_restriction.go).

namespace zonehelpers
{

// CharTargidHighThreshold is the exclusive upper bound for player targids
// (0x700). At or above this value, insert packets are ignored.
constexpr uint16 CharTargidHighThreshold = 0x700;

struct CharTargidAllocation
{
    uint16 targid;
    bool   high;
};

// AllocateCharTargid mirrors CZoneEntities::GetNewCharTargID. The supplied
// set is ordered like m_charTargIds; allocation starts at 0x400 and stops at
// the first value that does not equal the current candidate.
inline auto AllocateCharTargid(const std::set<uint16>& usedTargids) -> CharTargidAllocation
{
    uint16 targid = 0x400;
    for (const auto used : usedTargids)
    {
        if (targid != used)
        {
            break;
        }
        ++targid;
    }
    return { targid, targid >= CharTargidHighThreshold };
}

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
//
// Formula (slice 2949 dual-wire):
//   targid >= CharTargidHighThreshold
//
// CharTargidHighThreshold is pinned at 0x700 (same as Go zone.CharTargidHighThreshold).
// true  → reject insert (ShowError high-targid prefix; update packets ignored)
// false → accept insert path (InsertPC may proceed)
//
// Dual-wire of Go zone.ShouldRejectHighCharTargid.
// Call site: CZone::IncreaseZoneCounter after GetNewCharTargID assigns PChar->targid.
// Prior pure port: slice 1363 (zone policy suite). Residual pins remain in
// test_zone_policy_1363; dedicated dual-wire suite is test_zone_high_targid_2949.
inline auto ShouldRejectHighCharTargid(const uint16 targid) -> bool
{
    return targid >= CharTargidHighThreshold;
}

// ShouldCreateZoneTimers mirrors !zoneTimerToken && !CharListEmpty after insert.
//
// Formula (slice 2992 dual-wire):
//   !hasZoneTimerToken && !charListEmpty
//
// hasZoneTimerToken — host-evaluated zoneTimerToken_.has_value() after InsertPC
// charListEmpty     — host-evaluated m_zoneEntities->CharListEmpty() after InsertPC
// true  → createZoneTimers() (first PC joined with no active zone timer)
// false → timers already running, or char list still empty
//
// Dense 2² space: only (token=false, empty=false) is true; all other poles false.
//
// Dual-wire of Go zone.ShouldCreateZoneTimers.
// Call site: CZone::IncreaseZoneCounter after m_zoneEntities->InsertPC —
// host injects zoneTimerToken_.has_value() and CharListEmpty().
// Prior pure port: slice 1363 (zone policy suite). Residual pins remain in
// test_zone_policy_1363; dedicated dual-wire suite is test_zone_create_timers_2992.
// Sibling leave gates: ShouldStampZoneEmptyTime (2939), ShouldDespawnPCOnLeave (2975).
inline auto ShouldCreateZoneTimers(const bool hasZoneTimerToken, const bool charListEmpty) -> bool
{
    return !hasZoneTimerToken && !charListEmpty;
}

// ShouldStampZoneEmptyTime mirrors CharListEmpty after DecreaseZoneCounter.
//
// Formula (slice 2939 dual-wire):
//   charListEmpty
//
// charListEmpty — host-evaluated m_zoneEntities->CharListEmpty() after decrease
// true  → stamp m_timeZoneEmpty = timer::now() (zone idle-empty clock starts)
// false → else-branch: ShouldDespawnPCOnLeave may DespawnPC for remaining PCs
//
// Dual-wire of Go zone.ShouldStampZoneEmptyTime.
// Call site: CZone::DecreaseZoneCounter after DecreaseZoneCounter entities update.
// Prior pure port: slice 1363 (zone policy suite). Residual pins remain in
// test_zone_policy_1363; dedicated dual-wire suite is test_zone_stamp_empty_2939.
inline auto ShouldStampZoneEmptyTime(const bool charListEmpty) -> bool
{
    return charListEmpty;
}

// ShouldDespawnPCOnLeave mirrors !CharListEmpty after DecreaseZoneCounter
// (else-branch of the empty-stamp gate).
//
// Formula (slice 2975 dual-wire):
//   !charListEmpty
//
// charListEmpty — host-evaluated m_zoneEntities->CharListEmpty() after decrease
// true  → DespawnPC(PChar) for the leaving char (other PCs remain in zone)
// false → no despawn here; ShouldStampZoneEmptyTime stamps m_timeZoneEmpty
//
// Polarity note: opposite of ShouldStampZoneEmptyTime (true when NOT empty).
//
// Dual-wire of Go zone.ShouldDespawnPCOnLeave.
// Call site: CZone::DecreaseZoneCounter after ShouldStampZoneEmptyTime —
// host injects CharListEmpty(); else-branch calls DespawnPC(PChar).
// Prior pure port: slices 1363 / 2706 residual (zone policy suite). Residual
// pins remain in test_zone_policy_1363; dedicated dual-wire suite is
// test_zone_despawn_pc_leave_2975. Sibling: ShouldStampZoneEmptyTime (2939).
inline auto ShouldDespawnPCOnLeave(const bool charListEmpty) -> bool
{
    return !charListEmpty;
}

// ShouldStopZoneTimers mirrors CZone::ZoneServer's idle shutdown gate. The
// five-second delay is strict: exactly five seconds after the zone became
// empty is still retained.
inline auto ShouldStopZoneTimers(
    const bool              hasZoneTimer,
    const bool              charListEmpty,
    const timer::time_point zoneEmptyTime,
    const timer::time_point now,
    const bool              allMobsHomeAndHealed) -> bool
{
    return hasZoneTimer && charListEmpty && zoneEmptyTime + std::chrono::seconds(5) < now && allMobsHomeAndHealed;
}

struct ZoneTimerPlan
{
    bool                      install{};
    std::chrono::milliseconds logicInterval{};
    std::chrono::milliseconds triggerAreaInterval{};
};

// PlanZoneTimers mirrors CZone::createZoneTimers. Test servers manually
// advance ticks and therefore install neither recurring timer.
inline auto PlanZoneTimers(const bool isTestServer) -> ZoneTimerPlan
{
    if (isTestServer)
    {
        return {};
    }
    return { true, kLogicUpdateInterval, kTriggerAreaInterval };
}

// PlanSealTimerExpiry mirrors CharZoneOut's optional SealTimerExpiry save.
inline auto PlanSealTimerExpiry(const bool persistEnabled, const bool hasRecast, const timer::duration remaining, const uint32 now) -> std::optional<uint32>
{
    if (!persistEnabled || !hasRecast || remaining <= std::chrono::seconds(10))
    {
        return std::nullopt;
    }
    return now + static_cast<uint32>(std::chrono::duration_cast<std::chrono::seconds>(remaining).count());
}

struct SealTimerRestorePlan { bool restore{}; uint32 remainingSeconds{}; bool clearStoredExpiry{}; };
inline auto PlanSealTimerRestore(const bool persistEnabled, const uint32 expiry, const uint32 now) -> SealTimerRestorePlan
{
    if (!persistEnabled || expiry == 0) return {};
    const auto remaining = expiry > now ? expiry - now : 0;
    return { remaining > 0 && remaining <= 300, remaining, true };
}

template <typename Areas, typename IsMember>
auto FirstZoneOutTriggerArea(const Areas& areas, IsMember&& isMember) -> typename Areas::const_iterator
{
    for (auto it = areas.begin(); it != areas.end(); ++it) if (isMember((*it)->getTriggerAreaID())) return it;
    return areas.end();
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

// ShouldApplyZoneLevelRestriction mirrors m_levelRestriction != 0 on
// CZone::updateCharLevelRestriction admission (apply LevelRestriction when
// the zone has a non-zero cap).
//
// Formula (slice 3032 dual-wire):
//
//   zoneLevelRestriction != 0
//
// zoneLevelRestriction — host-injected m_levelRestriction (uint8)
// true  → DelStatusEffectsByFlag (dispelable/erasable/…) + AddStatusEffect LevelRestriction
// false → no apply (zone has no level cap)
//
// Dual-wire of Go zone.ShouldApplyZoneLevelRestriction.
// Call site: CZone::updateCharLevelRestriction — after optional skip/delete
// of an existing LevelRestriction; host injects m_levelRestriction.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_apply_level_restriction_3032.
// Sibling level-restriction gates (residual 1363): ShouldSkipLevelRestrictionUpdate,
// ShouldDeleteExistingLevelRestriction.
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

// ShouldRejectInvalidWeather mirrors !magic_enum::enum_contains<Weather>(weather)
// on CZone::SetWeather admission.
//
// Formula (slice 3019 dual-wire):
//
//   !isValidEnum
//
// isValidEnum — host-evaluated magic_enum::enum_contains<Weather>(weather)
// true  → ShowWarningFmt(FormatInvalidWeatherWarning) + return (reject set)
// false → continue (may still ShouldSkipSameWeather / WeatherChange)
//
// Dual-wire of Go zone.ShouldRejectInvalidWeather.
// Call site: CZone::SetWeather — host injects enum_contains result.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_reject_invalid_weather_3019.
// Sibling SetWeather gates: ShouldSkipSameWeather (3028 dual-wire),
// FormatInvalidWeatherWarning (residual 1363).
inline auto ShouldRejectInvalidWeather(const bool isValidEnum) -> bool
{
    return !isValidEnum;
}

// ShouldSkipSameWeather mirrors weather_.current() == weather on
// CZone::SetWeather admission (already-current identity skip).
//
// Formula (slice 3028 dual-wire):
//
//   alreadyCurrent
//
// alreadyCurrent — host-evaluated weather_.current() == weather
// true  → return early (no WeatherChange / set / packet)
// false → continue (WeatherChange + weather_.set + WEATHER packet)
//
// Dual-wire of Go zone.ShouldSkipSameWeather.
// Call site: CZone::SetWeather — after ShouldRejectInvalidWeather; host
// injects current==requested comparison.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_skip_same_weather_3028.
// Sibling SetWeather gates: ShouldRejectInvalidWeather (3019 dual-wire),
// FormatInvalidWeatherWarning (residual 1363).
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

inline auto ShouldDismountOnZoneIn(const bool mounted, const bool canUseMount) -> bool
{
    return mounted && !canUseMount;
}
inline auto ShouldClearCostumeOnZoneIn(const bool hasCostume) -> bool { return hasCostume; }

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
