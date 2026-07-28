#pragma once

#include "data/enums/latent.h"

#include <cstdint>

// Pure CLatentEffectContainer::CheckLatentsWeaponBreak dispatch predicate.
namespace latenthelpers
{

constexpr auto ShouldProcessWeaponBreakLatent(const xi::Latent condition, const std::uint16_t value, const std::uint8_t slot) -> bool
{
    return condition == xi::Latent::WeaponBroken && value == slot;
}

} // namespace latenthelpers
