#include "test_zone_policy_1363.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone policy 1363 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "zone policy 1363 self-test failed: " << label << " got "
                  << static_cast<long long>(actual) << " expected " << static_cast<long long>(expected) << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runZonePolicy1363SelfTests() -> bool
{
    bool ok = true;

    // Counter / insert
    ok = expect(zonehelpers::ShouldRejectIncreaseZoneCounter(true, false, false), "null char") && ok;
    ok = expect(zonehelpers::ShouldRejectIncreaseZoneCounter(false, true, false), "already zone") && ok;
    ok = expect(zonehelpers::ShouldRejectIncreaseZoneCounter(false, false, true), "has pool") && ok;
    ok = expect(!zonehelpers::ShouldRejectIncreaseZoneCounter(false, false, false), "accept enter") && ok;
    ok = expect(zonehelpers::FormatIncreaseZoneCounterWarning() ==
                    "CZone::IncreaseZoneCounter() - PChar is null, or Player zone or Treasure Pools is not null.",
                "enter warn") &&
         ok;
    ok = expect(zonehelpers::ShouldRejectHighCharTargid(0x700), "targid high") && ok;
    ok = expect(!zonehelpers::ShouldRejectHighCharTargid(0x6FF), "targid ok") && ok;
    ok = expectEq(zonehelpers::CharTargidHighThreshold, static_cast<uint16>(0x700), "targid thresh") && ok;
    ok = expect(zonehelpers::ShouldCreateZoneTimers(false, false), "create timers") && ok;
    ok = expect(!zonehelpers::ShouldCreateZoneTimers(true, false), "has token") && ok;
    ok = expect(zonehelpers::ShouldStampZoneEmptyTime(true), "empty stamp") && ok;
    ok = expect(zonehelpers::ShouldDespawnPCOnLeave(false), "despawn leave") && ok;

    // Level restriction
    ok = expect(zonehelpers::ShouldSkipLevelRestrictionUpdate(true, false, true), "skip same") && ok;
    ok = expect(zonehelpers::ShouldSkipLevelRestrictionUpdate(true, true, false), "skip null status") && ok;
    ok = expect(!zonehelpers::ShouldSkipLevelRestrictionUpdate(false, false, false), "no skip") && ok;
    ok = expect(zonehelpers::ShouldDeleteExistingLevelRestriction(true, false), "delete existing") && ok;
    ok = expect(!zonehelpers::ShouldDeleteExistingLevelRestriction(true, true), "no delete skip") && ok;
    ok = expect(zonehelpers::ShouldApplyZoneLevelRestriction(50), "apply cap") && ok;
    ok = expect(!zonehelpers::ShouldApplyZoneLevelRestriction(0), "no cap") && ok;

    // Weather
    ok = expect(zonehelpers::ShouldRejectInvalidWeather(false), "invalid weather") && ok;
    ok = expect(!zonehelpers::ShouldRejectInvalidWeather(true), "valid weather") && ok;
    ok = expect(zonehelpers::FormatInvalidWeatherWarning(99) == "Weather value (99) invalid.", "weather warn") && ok;
    ok = expect(zonehelpers::ShouldSkipSameWeather(true), "same weather") && ok;
    ok = expectEq(zonehelpers::WeatherPacketOffsetMin, static_cast<uint16>(4), "offset min") && ok;
    ok = expectEq(zonehelpers::WeatherPacketOffsetMaxExclusive, static_cast<uint16>(28), "offset max") && ok;
    ok = expectEq(zonehelpers::SelectWeatherBand(0), static_cast<uint8>(0), "rare") && ok;
    ok = expectEq(zonehelpers::SelectWeatherBand(14), static_cast<uint8>(0), "rare edge") && ok;
    ok = expectEq(zonehelpers::SelectWeatherBand(15), static_cast<uint8>(1), "common") && ok;
    ok = expectEq(zonehelpers::SelectWeatherBand(49), static_cast<uint8>(1), "common edge") && ok;
    ok = expectEq(zonehelpers::SelectWeatherBand(50), static_cast<uint8>(2), "normal") && ok;
    ok = expect(zonehelpers::ShouldForceMorningFog(true, true, false), "force fog") && ok;
    ok = expect(!zonehelpers::ShouldForceMorningFog(true, true, true), "city no fog") && ok;
    ok = expect(!zonehelpers::ShouldForceMorningFog(true, false, false), "elemental no fog") && ok;
    ok = expect(zonehelpers::ShouldRescheduleDynamicWeather(false), "reschedule") && ok;
    ok = expect(!zonehelpers::ShouldRescheduleDynamicWeather(true), "static no reschedule") && ok;
    ok = expectEq(zonehelpers::WeatherCycleDays, static_cast<uint32>(2160), "cycle") && ok;
    ok = expectEq(zonehelpers::WeatherUpdateDelayMinSeconds, 180, "delay min") && ok;
    ok = expectEq(zonehelpers::WeatherUpdateDelayMaxExclusiveSeconds, 1801, "delay max") && ok;

    // Mobs home
    ok = expect(zonehelpers::IsMobAwayFromHome(false, false, false), "away unhealed") && ok;
    ok = expect(zonehelpers::IsMobAwayFromHome(false, true, true), "away pathing") && ok;
    ok = expect(!zonehelpers::IsMobAwayFromHome(true, false, false), "dead home") && ok;
    ok = expect(!zonehelpers::IsMobAwayFromHome(false, true, false), "healed home") && ok;
    ok = expect(zonehelpers::ShouldReportAllMobsHomeAndHealed(false, false), "empty list ok") && ok;
    ok = expect(zonehelpers::ShouldReportAllMobsHomeAndHealed(true, false), "all home") && ok;
    ok = expect(!zonehelpers::ShouldReportAllMobsHomeAndHealed(true, true), "some away") && ok;

    // Misc
    ok = expect(zonehelpers::CanUseMisc(0x0000, 0x0000), "empty misc requirement") && ok;
    ok = expect(zonehelpers::CanUseMisc(0x0003, 0x0001), "misc ok") && ok;
    ok = expect(!zonehelpers::CanUseMisc(0x0001, 0x0003), "misc fail") && ok;
    ok = expect(zonehelpers::CanUseMisc(0xFFFF, 0xFFFF), "all misc flags") && ok;

    return ok;
}
