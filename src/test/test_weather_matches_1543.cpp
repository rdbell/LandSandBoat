#include "test_weather_matches_1543.h"

#include "map/weather_matches_capacity.h"

#include <iostream>

namespace
{
using weathermatcheshelpers::ElementDark;
using weathermatcheshelpers::ElementEarth;
using weathermatcheshelpers::ElementFire;
using weathermatcheshelpers::ElementIce;
using weathermatcheshelpers::ElementLight;
using weathermatcheshelpers::ElementNone;
using weathermatcheshelpers::ElementThunder;
using weathermatcheshelpers::ElementWater;
using weathermatcheshelpers::ElementWind;
using weathermatcheshelpers::WeatherMatchesElement;

auto Check() -> bool
{
    if (WeatherMatchesElement(Weather::HotSpell, ElementNone) || WeatherMatchesElement(Weather::Sunshine, ElementFire))
    {
        return false;
    }
    if (!WeatherMatchesElement(Weather::HotSpell, ElementFire) || !WeatherMatchesElement(Weather::HeatWave, ElementFire) ||
        !WeatherMatchesElement(Weather::Snow, ElementIce) || !WeatherMatchesElement(Weather::Blizzards, ElementIce) ||
        !WeatherMatchesElement(Weather::Wind, ElementWind) || !WeatherMatchesElement(Weather::Gales, ElementWind) ||
        !WeatherMatchesElement(Weather::DustStorm, ElementEarth) || !WeatherMatchesElement(Weather::SandStorm, ElementEarth) ||
        !WeatherMatchesElement(Weather::Thunder, ElementThunder) ||
        !WeatherMatchesElement(Weather::Thunderstorms, ElementThunder) ||
        !WeatherMatchesElement(Weather::Auroras, ElementLight) ||
        !WeatherMatchesElement(Weather::StellarGlare, ElementLight))
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
    if (WeatherMatchesElement(static_cast<Weather>(255), ElementDark))
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
