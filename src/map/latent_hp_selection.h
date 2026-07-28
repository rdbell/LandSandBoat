#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsHP condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessHpLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::HpUnderPercent:
        case xi::Latent::HpOverPercent:
        case xi::Latent::HpUnderTpUnder100:
        case xi::Latent::HpOverTpUnder100:
        case xi::Latent::SanctionRegenBonus:
        case xi::Latent::SigilRegenBonus:
        case xi::Latent::HpOverVisibleGear:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
