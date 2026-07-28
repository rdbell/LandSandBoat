#include "test_latent_weather_plan_7532.h"

#include "map/latent_weather_plan.h"

#include <cstdint>
#include <iostream>

auto runLatentWeatherPlan7532SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent weather plan 7532 self-test failed: " << label << '\n';
        }
        return value;
    };

    const auto expected = [](const xi::Latent condition, const std::uint16_t value, const std::uint16_t weather, const std::uint16_t element) {
        switch (condition)
        {
            case xi::Latent::WeatherCondition:
                return value == weather ? latenthelpers::WeatherLatentAction::Activate : latenthelpers::WeatherLatentAction::Deactivate;
            case xi::Latent::WeatherElement:
                return value == element ? latenthelpers::WeatherLatentAction::Activate : latenthelpers::WeatherLatentAction::Deactivate;
            default:
                return latenthelpers::WeatherLatentAction::Ignore;
        }
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        if (!expect(latenthelpers::DetermineWeatherLatentAction(condition, 3, 3, 4) == expected(condition, 3, 3, 4), "condition catalog"))
        {
            return false;
        }
    }

    return expect(latenthelpers::DetermineWeatherLatentAction(xi::Latent::WeatherCondition, 7, 7, 4) == latenthelpers::WeatherLatentAction::Activate, "weather match") &&
           expect(latenthelpers::DetermineWeatherLatentAction(xi::Latent::WeatherCondition, 7, 8, 4) == latenthelpers::WeatherLatentAction::Deactivate, "weather mismatch") &&
           expect(latenthelpers::DetermineWeatherLatentAction(xi::Latent::WeatherElement, 4, 7, 4) == latenthelpers::WeatherLatentAction::Activate, "element match") &&
           expect(latenthelpers::DetermineWeatherLatentAction(xi::Latent::WeatherElement, 4, 7, 5) == latenthelpers::WeatherLatentAction::Deactivate, "element mismatch");
}
