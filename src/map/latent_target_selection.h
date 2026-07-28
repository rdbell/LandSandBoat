#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsTargetChange condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessTargetLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::SignetBonus:
        case xi::Latent::VsEcosystem:
        case xi::Latent::VsSpecies:
        case xi::Latent::VsFamily:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
