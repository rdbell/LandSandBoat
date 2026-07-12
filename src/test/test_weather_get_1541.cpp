#include "test_weather_get_1541.h"

#include "map/weather_get_capacity.h"

#include <iostream>

namespace
{
using weathergethelpers::ResolveWeather;
using weathergethelpers::ScholarStorms;
using weathergethelpers::ScholarWeatherFromStorms;
using weathergethelpers::ShouldReturnNoneWeather;

auto Check() -> bool
{
    if (!ShouldReturnNoneWeather(false, true) || !ShouldReturnNoneWeather(true, false) || ShouldReturnNoneWeather(true, true))
    {
        return false;
    }

    if (ScholarWeatherFromStorms({}) != Weather::None)
    {
        return false;
    }
    {
        ScholarStorms s{};
        s.firestorm = true;
        if (ScholarWeatherFromStorms(s) != Weather::HotSpell)
        {
            return false;
        }
    }
    {
        ScholarStorms s{};
        s.voidstorm = true;
        if (ScholarWeatherFromStorms(s) != Weather::Gloom)
        {
            return false;
        }
    }
    {
        ScholarStorms s{};
        s.firestorm = true;
        s.voidstorm = true;
        if (ScholarWeatherFromStorms(s) != Weather::Gloom)
        {
            return false;
        }
    }
    {
        ScholarStorms s{};
        s.rainstorm = true;
        s.windstorm = true;
        if (ScholarWeatherFromStorms(s) != Weather::Wind)
        {
            return false;
        }
    }

    // Stack Rain + Rain → Squall
    if (ResolveWeather(Weather::Rain, Weather::Rain, false) != Weather::Squall)
    {
        return false;
    }
    // Strong zone overwrites scholar weak
    if (ResolveWeather(Weather::HeatWave, Weather::HotSpell, false) != Weather::HeatWave)
    {
        return false;
    }
    // Scholar overrides mismatch
    if (ResolveWeather(Weather::Rain, Weather::HotSpell, false) != Weather::HotSpell)
    {
        return false;
    }
    // ignoreScholar
    if (ResolveWeather(Weather::Sunshine, Weather::Rain, true) != Weather::Sunshine)
    {
        return false;
    }
    return true;
}
} // namespace

auto runWeatherGet1541SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "weather_get_1541 self-tests failed\n";
        return false;
    }
    return true;
}
