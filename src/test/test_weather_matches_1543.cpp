#include "test_weather_matches_1543.h"

#include "map/weather_matches_capacity.h"

#include <iostream>

namespace
{
using weathermatcheshelpers::ElementDark;
using weathermatcheshelpers::ElementFire;
using weathermatcheshelpers::ElementNone;
using weathermatcheshelpers::ElementWater;
using weathermatcheshelpers::WeatherMatchesElement;

auto Check() -> bool
{
    if (WeatherMatchesElement(Weather::HotSpell, ElementNone) || WeatherMatchesElement(Weather::Sunshine, ElementFire))
    {
        return false;
    }
    if (!WeatherMatchesElement(Weather::HotSpell, ElementFire) || !WeatherMatchesElement(Weather::HeatWave, ElementFire))
    {
        return false;
    }
    if (WeatherMatchesElement(Weather::Rain, ElementFire))
    {
        return false;
    }
    if (!WeatherMatchesElement(Weather::Rain, ElementWater) || !WeatherMatchesElement(Weather::Squall, ElementWater))
    {
        return false;
    }
    if (!WeatherMatchesElement(Weather::Gloom, ElementDark) || !WeatherMatchesElement(Weather::Darkness, ElementDark))
    {
        return false;
    }
    if (WeatherMatchesElement(Weather::HotSpell, 99))
    {
        return false;
    }
    return true;
}
} // namespace

auto runWeatherMatches1543SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "weather_matches_1543 self-tests failed\n";
        return false;
    }
    return true;
}
