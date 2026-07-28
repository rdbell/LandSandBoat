#include "test_latent_target_selection_7519.h"

#include "map/latent_target_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentTargetSelection7519SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent target selection 7519 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array targetConditions{
        xi::Latent::SignetBonus,
        xi::Latent::VsEcosystem,
        xi::Latent::VsSpecies,
        xi::Latent::VsFamily,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(targetConditions.begin(), targetConditions.end(), condition) != targetConditions.end();
        if (!expect(latenthelpers::ShouldProcessTargetLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
