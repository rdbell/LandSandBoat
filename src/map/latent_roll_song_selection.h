#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsRollSong condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessRollSongLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::SongRollActive:
        case xi::Latent::ElevenRollActive:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
