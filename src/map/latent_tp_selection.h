#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsTP condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessTpLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::TpUnder:
        case xi::Latent::TpOver:
        case xi::Latent::HpUnderTpUnder100:
        case xi::Latent::HpOverTpUnder100:
        case xi::Latent::SanctionRefreshBonus:
        case xi::Latent::SigilRefreshBonus:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
