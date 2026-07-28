#pragma once

#include "data/enums/latent.h"

#include <cstdint>

// Pure CLatentEffectContainer::CheckLatentsWeaponDraw action plan.
namespace latenthelpers
{

enum class WeaponDrawLatentAction
{
    Ignore,
    Activate,
    Deactivate,
};

constexpr auto DetermineWeaponDrawLatentAction(const xi::Latent condition,
                                                const bool       drawn,
                                                const std::int32_t hp,
                                                const std::int32_t mp,
                                                const std::uint16_t value) -> WeaponDrawLatentAction
{
    if (!drawn)
    {
        switch (condition)
        {
            case xi::Latent::WeaponDrawn:
            case xi::Latent::WeaponDrawnMpOver:
            case xi::Latent::WeaponDrawnHpUnder:
                return WeaponDrawLatentAction::Deactivate;
            case xi::Latent::WeaponSheathed:
                return WeaponDrawLatentAction::Activate;
            default:
                return WeaponDrawLatentAction::Ignore;
        }
    }

    switch (condition)
    {
        case xi::Latent::WeaponDrawn:
            return WeaponDrawLatentAction::Activate;
        case xi::Latent::WeaponDrawnMpOver:
            return mp > value ? WeaponDrawLatentAction::Activate : WeaponDrawLatentAction::Deactivate;
        case xi::Latent::WeaponDrawnHpUnder:
            return hp < value ? WeaponDrawLatentAction::Activate : WeaponDrawLatentAction::Deactivate;
        case xi::Latent::WeaponSheathed:
            return WeaponDrawLatentAction::Deactivate;
        default:
            return WeaponDrawLatentAction::Ignore;
    }
}

} // namespace latenthelpers
