#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsPartyAvatar condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessPartyAvatarLatent(const xi::Latent condition) -> bool
{
    return condition == xi::Latent::AvatarInParty;
}

} // namespace latenthelpers
