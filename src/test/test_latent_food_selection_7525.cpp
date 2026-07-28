#include "test_latent_food_selection_7525.h"

#include "map/latent_food_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentFoodSelection7525SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent food selection 7525 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array foodConditions{
        xi::Latent::FoodActive,
        xi::Latent::NoFoodActive,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(foodConditions.begin(), foodConditions.end(), condition) != foodConditions.end();
        if (!expect(latenthelpers::ShouldProcessFoodLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
