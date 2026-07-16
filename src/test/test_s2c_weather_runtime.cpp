#include "test_s2c_weather_runtime.h"

#include <iostream>

#include "map/enums/weather.h"
#include "map/packets/s2c/weather_runtime.h"

auto runS2CWeatherRuntimeSelfTests() -> bool
{
    const auto plan = weatherhelpers::PlanFor({
        .startTime  = 0x11223344,
        .weather    = Weather::Blizzards,
        .offsetTime = 0x5566,
    });

    const auto matches = plan.StartTime == 0x11223344 &&
                         plan.WeatherNumber == Weather::Blizzards &&
                         plan.WeatherOffsetTime == 0x5566;
    if (!matches)
    {
        std::cerr << "s2c WEATHER runtime self-test failed: constructor fields\n";
    }
    return matches;
}
