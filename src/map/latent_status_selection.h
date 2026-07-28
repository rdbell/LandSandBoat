#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsStatusEffect condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessStatusLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::StatusEffectActive:
        case xi::Latent::WeatherCondition:
        case xi::Latent::WeatherElement:
        case xi::Latent::NationControl:
        case xi::Latent::InGarrison:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
