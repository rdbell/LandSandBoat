#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsFoodEffect condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessFoodLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::FoodActive:
        case xi::Latent::NoFoodActive:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
