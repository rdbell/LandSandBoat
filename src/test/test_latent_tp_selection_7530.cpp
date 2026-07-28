#include "test_latent_tp_selection_7530.h"

#include "map/latent_tp_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentTpSelection7530SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent TP selection 7530 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array tpConditions{
        xi::Latent::TpUnder,
        xi::Latent::TpOver,
        xi::Latent::HpUnderTpUnder100,
        xi::Latent::HpOverTpUnder100,
        xi::Latent::SanctionRefreshBonus,
        xi::Latent::SigilRefreshBonus,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(tpConditions.begin(), tpConditions.end(), condition) != tpConditions.end();
        if (!expect(latenthelpers::ShouldProcessTpLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
