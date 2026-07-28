#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer::CheckLatentsZone condition selector.
namespace latenthelpers
{

constexpr auto ShouldProcessZoneLatent(const xi::Latent condition) -> bool
{
    switch (condition)
    {
        case xi::Latent::Zone:
        case xi::Latent::InAssault:
        case xi::Latent::InDynamis:
        case xi::Latent::InAdoulin:
        case xi::Latent::WeatherCondition:
        case xi::Latent::WeatherElement:
        case xi::Latent::NationControl:
        case xi::Latent::NationCitizen:
        case xi::Latent::ZoneHomeNation:
            return true;
        default:
            return false;
    }
}

} // namespace latenthelpers
