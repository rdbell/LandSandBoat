#pragma once

#include "enums/weather.h"

#include <cstdint>

// Pure WeatherMatchesElement (parity with internal/attackutils).

namespace weathermatcheshelpers
{

constexpr std::uint8_t ElementNone    = 0;
constexpr std::uint8_t ElementFire    = 1;
constexpr std::uint8_t ElementIce     = 2;
constexpr std::uint8_t ElementWind    = 3;
constexpr std::uint8_t ElementEarth   = 4;
constexpr std::uint8_t ElementThunder = 5;
constexpr std::uint8_t ElementWater   = 6;
constexpr std::uint8_t ElementLight   = 7;
constexpr std::uint8_t ElementDark    = 8;

constexpr auto WeatherMatchesElement(const Weather weather, const std::uint8_t element) -> bool
{
    switch (element)
    {
        case ElementNone:
            return false;
        case ElementFire:
            return weather == Weather::HotSpell || weather == Weather::HeatWave;
        case ElementIce:
            return weather == Weather::Snow || weather == Weather::Blizzards;
        case ElementWind:
            return weather == Weather::Wind || weather == Weather::Gales;
        case ElementEarth:
            return weather == Weather::DustStorm || weather == Weather::SandStorm;
        case ElementThunder:
            return weather == Weather::Thunder || weather == Weather::Thunderstorms;
        case ElementWater:
            return weather == Weather::Rain || weather == Weather::Squall;
        case ElementLight:
            return weather == Weather::Auroras || weather == Weather::StellarGlare;
        case ElementDark:
            return weather == Weather::Gloom || weather == Weather::Darkness;
        default:
            return false;
    }
}

} // namespace weathermatcheshelpers
