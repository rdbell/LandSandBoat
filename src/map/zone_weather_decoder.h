/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include "weather_container.h"

#include <optional>
#include <span>
#include <utility>

// Helpers for decoding the packed per-day zone_weather database blob.
namespace zoneweatherhelpers
{

// DecodeEntry returns no entry for the all-zero sentinel. Nonzero values store
// normal, common, and rare weather in three five-bit fields, high to low.
inline auto DecodeEntry(const uint16 packed) -> std::optional<ZoneWeather>
{
    if (packed == 0)
    {
        return std::nullopt;
    }

    return ZoneWeather(
        static_cast<Weather>(packed >> 10),
        static_cast<Weather>((packed >> 5) & 0x1F),
        static_cast<Weather>(packed & 0x1F));
}

// DecodeBlob emits an entry for every nonzero day, retaining the source blob's
// day order. CZone owns insertion into its probability table.
template <typename Append>
void DecodeBlob(const std::span<const uint16> weatherBlob, Append&& append)
{
    for (size_t day = 0; day < weatherBlob.size(); ++day)
    {
        if (const auto entry = DecodeEntry(weatherBlob[day]))
        {
            std::forward<Append>(append)(static_cast<uint16>(day), *entry);
        }
    }
}

} // namespace zoneweatherhelpers
