#pragma once

#include "enums/weather.h"

#include <cstdint>

// Pure GetWeather scholar-storm mapping and null/zone gate.
// Resolve stacking uses the same rules as OmegaXI weathercontainer::ResolveWeather.

namespace weathergethelpers
{

constexpr auto ShouldReturnNoneWeather(const bool hasEntity, const bool hasZone) -> bool
{
    return !hasEntity || !hasZone;
}

struct ScholarStorms
{
    bool firestorm    = false;
    bool rainstorm    = false;
    bool sandstorm    = false;
    bool windstorm    = false;
    bool hailstorm    = false;
    bool thunderstorm = false;
    bool aurorastorm  = false;
    bool voidstorm    = false;
};

// Sequential ifs: last true storm wins (matches battleutils::GetWeather).
constexpr auto ScholarWeatherFromStorms(const ScholarStorms s) -> Weather
{
    auto w = Weather::None;
    if (s.firestorm)
    {
        w = Weather::HotSpell;
    }
    if (s.rainstorm)
    {
        w = Weather::Rain;
    }
    if (s.sandstorm)
    {
        w = Weather::DustStorm;
    }
    if (s.windstorm)
    {
        w = Weather::Wind;
    }
    if (s.hailstorm)
    {
        w = Weather::Snow;
    }
    if (s.thunderstorm)
    {
        w = Weather::Thunder;
    }
    if (s.aurorastorm)
    {
        w = Weather::Auroras;
    }
    if (s.voidstorm)
    {
        w = Weather::Gloom;
    }
    return w;
}

// Mirrors ResolveWeather / battleutils stacking after scholar weather is known.
constexpr auto ResolveWeather(const Weather zoneWeather, const Weather scholarWeather, const bool ignoreScholar) -> Weather
{
    if (ignoreScholar || scholarWeather == Weather::None ||
        static_cast<std::uint16_t>(zoneWeather) == static_cast<std::uint16_t>(scholarWeather) + 1)
    {
        return zoneWeather;
    }
    if (scholarWeather == zoneWeather)
    {
        return static_cast<Weather>(static_cast<std::uint16_t>(zoneWeather) + 1);
    }
    return scholarWeather;
}

} // namespace weathergethelpers
