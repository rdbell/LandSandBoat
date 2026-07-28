#include "test_latent_hp_selection_7529.h"

#include "map/latent_hp_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentHpSelection7529SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent HP selection 7529 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array hpConditions{
        xi::Latent::HpUnderPercent,
        xi::Latent::HpOverPercent,
        xi::Latent::HpUnderTpUnder100,
        xi::Latent::HpOverTpUnder100,
        xi::Latent::SanctionRegenBonus,
        xi::Latent::SigilRegenBonus,
        xi::Latent::HpOverVisibleGear,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(hpConditions.begin(), hpConditions.end(), condition) != hpConditions.end();
        if (!expect(latenthelpers::ShouldProcessHpLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
