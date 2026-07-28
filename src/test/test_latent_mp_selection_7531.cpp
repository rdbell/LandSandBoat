#include "test_latent_mp_selection_7531.h"

#include "map/latent_mp_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentMpSelection7531SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent MP selection 7531 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array mpConditions{
        xi::Latent::MpUnderPercent,
        xi::Latent::MpUnder,
        xi::Latent::MpOver,
        xi::Latent::WeaponDrawnMpOver,
        xi::Latent::MpUnderVisibleGear,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(mpConditions.begin(), mpConditions.end(), condition) != mpConditions.end();
        if (!expect(latenthelpers::ShouldProcessMpLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
