#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsWS action plan.
namespace latenthelpers
{

enum class DuringWsLatentAction
{
    Ignore,
    Activate,
    Deactivate,
};

constexpr auto DetermineDuringWsLatentAction(const xi::Latent condition, const bool isDuringWs) -> DuringWsLatentAction
{
    if (condition != xi::Latent::DuringWs)
    {
        return DuringWsLatentAction::Ignore;
    }

    return isDuringWs ? DuringWsLatentAction::Activate : DuringWsLatentAction::Deactivate;
}

} // namespace latenthelpers
