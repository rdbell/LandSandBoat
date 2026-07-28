#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsJobLevel condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessJobLevelLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::JobMultiple:
        case xi::Latent::JobMultipleAtNight:
        case xi::Latent::JobLevelBelow:
        case xi::Latent::JobLevelAbove:
        case xi::Latent::InGarrison:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
