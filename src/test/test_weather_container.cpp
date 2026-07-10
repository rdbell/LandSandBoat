/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_weather_container.h"

#include "enums/weather.h"
#include "weather_container.h"

#include "common/vanadiel_clock.h"

#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

namespace
{

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "weather container self-test failed: " << label << " got "
                  << static_cast<int>(actual) << " expected " << static_cast<int>(expected) << '\n';
        return false;
    }
    return true;
}

auto expectEntry(const ZoneWeather& actual, const ZoneWeather& expected, const char* label) -> bool
{
    bool ok = true;
    ok      = expectEqual(actual.normal, expected.normal, label) && ok;
    ok      = expectEqual(actual.common, expected.common, label) && ok;
    ok      = expectEqual(actual.rare, expected.rare, label) && ok;
    return ok;
}

struct TransitionResult
{
    Weather              weather;
    std::chrono::seconds nextUpdate;
    bool                 forcedFog;
};

// Characterization seam for CZone::UpdateWeather. Keep this expression in
// lockstep with zone.cpp so its random and time boundaries remain reviewable
// without constructing the entire map-zone runtime in this unit fixture.
auto planTransition(const ZoneWeather& entry, const uint8 chance, const std::chrono::seconds randomDelay,
                    const xi::vanadiel_clock::duration sinceMidnight, const bool city) -> TransitionResult
{
    auto selectedWeather = Weather::None;
    if (chance < 15)
    {
        selectedWeather = entry.rare;
    }
    else if (chance < 50)
    {
        selectedWeather = entry.common;
    }
    else
    {
        selectedWeather = entry.normal;
    }

    auto nextUpdate = randomDelay;
    auto forcedFog  = false;
    if (sinceMidnight >= xi::vanadiel_clock::hours(2) && sinceMidnight < xi::vanadiel_clock::hours(7) &&
        selectedWeather < Weather::HotSpell && !city)
    {
        selectedWeather = Weather::Fog;
        nextUpdate       = std::chrono::duration_cast<std::chrono::seconds>(xi::vanadiel_clock::hours(7) - sinceMidnight);
        forcedFog        = true;
    }
    return { selectedWeather, nextUpdate, forcedFog };
}

auto testWeatherContainerDefaultsAndSet() -> bool
{
    WeatherContainer container;

    bool ok = true;
    ok      = expectEqual(container.current(), Weather::None, "default current weather") && ok;
    ok      = expectEqual(container.changeTime(), static_cast<uint32>(0), "default change time") && ok;
    ok      = expectEqual(container.isStatic(), true, "empty container is static") && ok;
    ok      = expectEntry(container.entryForDay(123), ZoneWeather(Weather::None, Weather::None, Weather::None), "empty entry lookup") && ok;

    container.set(Weather::Rain, 98765);

    ok = expectEqual(container.current(), Weather::Rain, "updated current weather") && ok;
    ok = expectEqual(container.changeTime(), static_cast<uint32>(98765), "updated change time") && ok;
    return ok;
}

auto testWeatherContainerProbabilityTable() -> bool
{
    WeatherContainer container;
    ZoneWeather      early(Weather::Sunshine, Weather::Clouds, Weather::Fog);
    ZoneWeather      mid(Weather::Rain, Weather::Squall, Weather::Thunder);
    ZoneWeather      late(Weather::Snow, Weather::Blizzards, Weather::Auroras);

    container.addEntry(10, mid);

    bool ok = true;
    ok      = expectEqual(container.isStatic(), true, "single-entry container is static") && ok;
    ok      = expectEntry(container.entryForDay(9), ZoneWeather(Weather::None, Weather::None, Weather::None), "lookup before first day") && ok;
    ok      = expectEntry(container.entryForDay(10), mid, "lookup first entry boundary") && ok;
    ok      = expectEntry(container.entryForDay(12), mid, "lookup after first entry") && ok;

    container.addEntry(3, early);
    container.addEntry(20, late);

    ok = expectEqual(container.isStatic(), false, "multi-entry container is not static") && ok;
    ok = expectEntry(container.entryForDay(2), ZoneWeather(Weather::None, Weather::None, Weather::None), "lookup before earliest day") && ok;
    ok = expectEntry(container.entryForDay(3), early, "lookup earliest boundary") && ok;
    ok = expectEntry(container.entryForDay(9), early, "lookup between earliest and middle") && ok;
    ok = expectEntry(container.entryForDay(10), mid, "lookup middle boundary") && ok;
    ok = expectEntry(container.entryForDay(19), mid, "lookup between middle and latest") && ok;
    ok = expectEntry(container.entryForDay(20), late, "lookup latest boundary") && ok;
    ok = expectEntry(container.entryForDay(65535), late, "lookup after latest") && ok;
    return ok;
}

auto testWeatherContainerDuplicateDayKeepsFirstEntry() -> bool
{
    WeatherContainer container;
    ZoneWeather      first(Weather::Sunshine, Weather::Clouds, Weather::Fog);
    ZoneWeather      second(Weather::Rain, Weather::Squall, Weather::Thunder);

    container.addEntry(7, first);
    container.addEntry(7, second);

    return expectEntry(container.entryForDay(7), first, "duplicate day keeps first inserted entry");
}

auto testWeatherSelectionBoundaries() -> bool
{
    const ZoneWeather entry(Weather::Sunshine, Weather::Clouds, Weather::Rain);
    const std::vector<std::pair<uint8, Weather>> cases = {
        { 0, Weather::Rain },       { 14, Weather::Rain },     { 15, Weather::Clouds },
        { 49, Weather::Clouds },    { 50, Weather::Sunshine }, { 99, Weather::Sunshine },
    };

    bool ok = true;
    for (const auto& [chance, expected] : cases)
    {
        const auto result = planTransition(entry, chance, std::chrono::seconds(180), xi::vanadiel_clock::hours(12), true);
        ok                = expectEqual(result.weather, expected, "weather chance boundary") && ok;
        ok                = expectEqual(result.nextUpdate.count(), static_cast<int64>(180), "random duration retained") && ok;
        ok                = expectEqual(result.forcedFog, false, "daytime weather is not forced") && ok;
    }
    return ok;
}

auto testForcedFogBoundaries() -> bool
{
    const ZoneWeather entry(Weather::Sunshine, Weather::Clouds, Weather::Rain);
    bool              ok = true;

    const auto before = planTransition(entry, 99, std::chrono::seconds(180), xi::vanadiel_clock::hours(2) - xi::vanadiel_clock::seconds(1), false);
    ok                = expectEqual(before.weather, Weather::Sunshine, "fog before 2am") && ok;
    ok                = expectEqual(before.nextUpdate.count(), static_cast<int64>(180), "pre-fog duration") && ok;

    const auto start = planTransition(entry, 99, std::chrono::seconds(180), xi::vanadiel_clock::hours(2), false);
    ok               = expectEqual(start.weather, Weather::Fog, "fog at 2am") && ok;
    ok               = expectEqual(start.nextUpdate.count(), static_cast<int64>(12 * 60), "fog duration to 7am") && ok;
    ok               = expectEqual(start.forcedFog, true, "fog start forced") && ok;

    const auto end = planTransition(entry, 99, std::chrono::seconds(180), xi::vanadiel_clock::hours(7), false);
    ok             = expectEqual(end.weather, Weather::Sunshine, "fog excluded at 7am") && ok;

    const auto city = planTransition(entry, 99, std::chrono::seconds(180), xi::vanadiel_clock::hours(3), true);
    ok              = expectEqual(city.weather, Weather::Sunshine, "city does not force fog") && ok;

    const ZoneWeather elemental(Weather::HotSpell, Weather::Clouds, Weather::Rain);
    const auto        hotSpell = planTransition(elemental, 99, std::chrono::seconds(180), xi::vanadiel_clock::hours(3), false);
    ok                         = expectEqual(hotSpell.weather, Weather::HotSpell, "elemental weather does not force fog") && ok;
    return ok;
}

} // namespace

auto runWeatherContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testWeatherContainerDefaultsAndSet() && ok;
    ok      = testWeatherContainerProbabilityTable() && ok;
    ok      = testWeatherContainerDuplicateDayKeepsFirstEntry() && ok;
    ok      = testWeatherSelectionBoundaries() && ok;
    ok      = testForcedFogBoundaries() && ok;
    return ok;
}
