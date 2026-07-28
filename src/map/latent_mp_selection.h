#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsMP condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessMpLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::MpUnderPercent:
        case xi::Latent::MpUnder:
        case xi::Latent::MpOver:
        case xi::Latent::WeaponDrawnMpOver:
        case xi::Latent::MpUnderVisibleGear:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
