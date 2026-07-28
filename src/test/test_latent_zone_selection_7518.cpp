#include "test_latent_zone_selection_7518.h"

#include "map/latent_zone_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentZoneSelection7518SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent zone selection 7518 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array zoneConditions{
        xi::Latent::Zone,
        xi::Latent::InAssault,
        xi::Latent::InDynamis,
        xi::Latent::InAdoulin,
        xi::Latent::WeatherCondition,
        xi::Latent::WeatherElement,
        xi::Latent::NationControl,
        xi::Latent::NationCitizen,
        xi::Latent::ZoneHomeNation,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(zoneConditions.begin(), zoneConditions.end(), condition) != zoneConditions.end();
        if (!expect(latenthelpers::ShouldProcessZoneLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
