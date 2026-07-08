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

#include <iostream>

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

} // namespace

auto runWeatherContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testWeatherContainerDefaultsAndSet() && ok;
    ok      = testWeatherContainerProbabilityTable() && ok;
    ok      = testWeatherContainerDuplicateDayKeepsFirstEntry() && ok;
    return ok;
}
