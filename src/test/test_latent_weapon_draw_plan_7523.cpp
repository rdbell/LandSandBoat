#include "test_latent_weapon_draw_plan_7523.h"

#include "map/latent_weapon_draw_plan.h"

#include <cstdint>
#include <iostream>

auto runLatentWeaponDrawPlan7523SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent weapon-draw plan 7523 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::uint16_t value = 50;
    const auto expected = [](const xi::Latent condition, const bool drawn, const std::int32_t hp, const std::int32_t mp) {
        if (!drawn)
        {
            switch (condition)
            {
                case xi::Latent::WeaponDrawn:
                case xi::Latent::WeaponDrawnMpOver:
                case xi::Latent::WeaponDrawnHpUnder:
                    return latenthelpers::WeaponDrawLatentAction::Deactivate;
                case xi::Latent::WeaponSheathed:
                    return latenthelpers::WeaponDrawLatentAction::Activate;
                default:
                    return latenthelpers::WeaponDrawLatentAction::Ignore;
            }
        }

        switch (condition)
        {
            case xi::Latent::WeaponDrawn:
                return latenthelpers::WeaponDrawLatentAction::Activate;
            case xi::Latent::WeaponDrawnMpOver:
                return mp > value ? latenthelpers::WeaponDrawLatentAction::Activate : latenthelpers::WeaponDrawLatentAction::Deactivate;
            case xi::Latent::WeaponDrawnHpUnder:
                return hp < value ? latenthelpers::WeaponDrawLatentAction::Activate : latenthelpers::WeaponDrawLatentAction::Deactivate;
            case xi::Latent::WeaponSheathed:
                return latenthelpers::WeaponDrawLatentAction::Deactivate;
            default:
                return latenthelpers::WeaponDrawLatentAction::Ignore;
        }
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        if (!expect(latenthelpers::DetermineWeaponDrawLatentAction(condition, true, 49, 51, value) == expected(condition, true, 49, 51), "drawn condition catalog") ||
            !expect(latenthelpers::DetermineWeaponDrawLatentAction(condition, false, 49, 51, value) == expected(condition, false, 49, 51), "sheathed condition catalog"))
        {
            return false;
        }
    }

    return expect(latenthelpers::DetermineWeaponDrawLatentAction(xi::Latent::WeaponDrawnMpOver, true, 0, 50, value) == latenthelpers::WeaponDrawLatentAction::Deactivate, "MP equality deactivates") &&
           expect(latenthelpers::DetermineWeaponDrawLatentAction(xi::Latent::WeaponDrawnMpOver, true, 0, 51, value) == latenthelpers::WeaponDrawLatentAction::Activate, "MP above activates") &&
           expect(latenthelpers::DetermineWeaponDrawLatentAction(xi::Latent::WeaponDrawnHpUnder, true, 50, 0, value) == latenthelpers::WeaponDrawLatentAction::Deactivate, "HP equality deactivates") &&
           expect(latenthelpers::DetermineWeaponDrawLatentAction(xi::Latent::WeaponDrawnHpUnder, true, 49, 0, value) == latenthelpers::WeaponDrawLatentAction::Activate, "HP below activates");
}
