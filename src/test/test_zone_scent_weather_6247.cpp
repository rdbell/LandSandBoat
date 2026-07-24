#include "test_zone_scent_weather_6247.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone scent weather 6247 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pins CZoneEntities::WeatherChange's scent-disable weather set. The production
// method only mutates m_disableScent when DETECT_SCENT is present; this helper
// supplies its weather membership test.
auto runZoneScentWeather6247SelfTests() -> bool
{
    using zonehelpers::ShouldDisableScentForWeather;

    bool ok = true;
    ok = expect(ShouldDisableScentForWeather(Weather::Rain), "rain disables scent") && ok;
    ok = expect(ShouldDisableScentForWeather(Weather::Squall), "squall disables scent") && ok;
    ok = expect(ShouldDisableScentForWeather(Weather::Blizzards), "blizzards disable scent") && ok;

    for (const auto weather : { Weather::None, Weather::Fog, Weather::Snow, Weather::Thunderstorms, Weather::Darkness })
    {
        ok = expect(!ShouldDisableScentForWeather(weather), "other weather preserves scent") && ok;
    }

    return ok;
}
