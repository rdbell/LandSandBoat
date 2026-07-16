#pragma once

#include "0x057_weather.h"

namespace weatherhelpers
{

struct Facts
{
    uint32  startTime{};
    Weather weather{};
    uint16  offsetTime{};
};

[[nodiscard]] constexpr auto PlanFor(const Facts& f) -> GP_SERV_COMMAND_WEATHER::PacketData
{
    return { .StartTime = f.startTime, .WeatherNumber = f.weather, .WeatherOffsetTime = f.offsetTime };
}

} // namespace weatherhelpers
