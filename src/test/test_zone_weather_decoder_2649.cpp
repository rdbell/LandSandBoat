/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_zone_weather_decoder_2649.h"

#include "map/enums/weather.h"
#include "map/zone_weather_decoder.h"

#include <array>
#include <iostream>
#include <utility>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone weather decoder 2649 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testDecodeBlobSkipsZeroAndRetainsDayBounds() -> bool
{
    constexpr size_t WeatherCycle = 2160;
    std::array<uint16, WeatherCycle> blob{};
    blob[0]                = static_cast<uint16>((static_cast<uint16>(Weather::Darkness) << 10) |
                                   (static_cast<uint16>(Weather::Snow) << 5) |
                                   static_cast<uint16>(Weather::Fog));
    blob[WeatherCycle - 1] = static_cast<uint16>((static_cast<uint16>(Weather::Rain) << 10) |
                                   (static_cast<uint16>(Weather::Gales) << 5) |
                                   static_cast<uint16>(Weather::Auroras));

    std::vector<std::pair<uint16, ZoneWeather>> decoded;
    zoneweatherhelpers::DecodeBlob(blob, [&decoded](const uint16 day, const ZoneWeather entry) {
        decoded.emplace_back(day, entry);
    });

    if (!expect(decoded.size() == 2, "zero words are skipped"))
    {
        return false;
    }

    bool ok = true;
    ok      = expect(decoded[0].first == 0, "first day retained") && ok;
    ok      = expect(decoded[0].second.normal == Weather::Darkness, "normal field is high five bits") && ok;
    ok      = expect(decoded[0].second.common == Weather::Snow, "common field is middle five bits") && ok;
    ok      = expect(decoded[0].second.rare == Weather::Fog, "rare field is low five bits") && ok;
    ok      = expect(decoded[1].first == WeatherCycle - 1, "last day retained") && ok;
    ok      = expect(decoded[1].second.normal == Weather::Rain, "last day normal decoded") && ok;
    ok      = expect(decoded[1].second.common == Weather::Gales, "last day common decoded") && ok;
    ok      = expect(decoded[1].second.rare == Weather::Auroras, "last day rare decoded") && ok;
    return ok;
}

} // namespace

auto runZoneWeatherDecoder2649SelfTests() -> bool
{
    return testDecodeBlobSkipsZeroAndRetainsDayBounds();
}
