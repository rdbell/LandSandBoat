#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsPartyJobs condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessPartyJobLatent(const xi::Latent condition) -> bool
{
    return condition == xi::Latent::JobInParty;
}

} // namespace latenthelpers
