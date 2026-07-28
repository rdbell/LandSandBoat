#include "test_latent_weapon_break_selection_7527.h"

#include "map/latent_weapon_break_selection.h"

#include <cstdint>
#include <iostream>

auto runLatentWeaponBreakSelection7527SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent weapon-break selection 7527 self-test failed: " << label << '\n';
        }
        return value;
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = condition == xi::Latent::WeaponBroken;
        if (!expect(latenthelpers::ShouldProcessWeaponBreakLatent(condition, 4, 4) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return expect(latenthelpers::ShouldProcessWeaponBreakLatent(xi::Latent::WeaponBroken, 0, 0), "first slot") &&
           expect(latenthelpers::ShouldProcessWeaponBreakLatent(xi::Latent::WeaponBroken, 17, 17), "last equipment slot") &&
           expect(!latenthelpers::ShouldProcessWeaponBreakLatent(xi::Latent::WeaponBroken, 4, 5), "slot mismatch") &&
           expect(!latenthelpers::ShouldProcessWeaponBreakLatent(xi::Latent::WeaponBroken, 260, 4), "large value does not truncate to slot") &&
           expect(!latenthelpers::ShouldProcessWeaponBreakLatent(xi::Latent::FoodActive, 4, 4), "other condition");
}
