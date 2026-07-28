#include "test_latent_status_selection_7521.h"

#include "map/latent_status_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentStatusSelection7521SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent status selection 7521 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array statusConditions{
        xi::Latent::StatusEffectActive,
        xi::Latent::WeatherCondition,
        xi::Latent::WeatherElement,
        xi::Latent::NationControl,
        xi::Latent::InGarrison,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(statusConditions.begin(), statusConditions.end(), condition) != statusConditions.end();
        if (!expect(latenthelpers::ShouldProcessStatusLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
