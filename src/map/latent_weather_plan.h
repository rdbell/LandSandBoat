#pragma once

#include "data/enums/latent.h"

#include <cstdint>

// Pure CLatentEffectContainer::CheckLatentsWeather action plan.
namespace latenthelpers
{

enum class WeatherLatentAction
{
    Ignore,
    Activate,
    Deactivate,
};

constexpr auto DetermineWeatherLatentAction(const xi::Latent condition,
                                            const std::uint16_t value,
                                            const std::uint16_t weather,
                                            const std::uint16_t element) -> WeatherLatentAction
{
    switch (condition)
    {
        case xi::Latent::WeatherCondition:
            return value == weather ? WeatherLatentAction::Activate : WeatherLatentAction::Deactivate;
        case xi::Latent::WeatherElement:
            return value == element ? WeatherLatentAction::Activate : WeatherLatentAction::Deactivate;
        default:
            return WeatherLatentAction::Ignore;
    }
}

} // namespace latenthelpers
