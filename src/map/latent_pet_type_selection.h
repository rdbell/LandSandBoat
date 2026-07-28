#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsPetType condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessPetTypeLatent(const xi::Latent condition) -> bool
{
    return condition == xi::Latent::PetId;
}

} // namespace latenthelpers
