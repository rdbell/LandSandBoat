#pragma once

#include "data/enums/latent.h"

// Pure CLatentEffectContainer time-refresh condition selector.
namespace latenthelpers
{

enum class LatentTimeRefresh
{
    Day,
    MoonPhase,
    WeekDay,
    Hour,
};

constexpr auto ShouldProcessTimeLatent(const LatentTimeRefresh refresh, const xi::Latent condition) -> bool
{
    switch (refresh)
    {
        case LatentTimeRefresh::Day:
            return condition == xi::Latent::TimeOfDay;
        case LatentTimeRefresh::MoonPhase:
            return condition == xi::Latent::MoonPhase;
        case LatentTimeRefresh::WeekDay:
            switch (condition)
            {
                case xi::Latent::Firesday:
                case xi::Latent::Earthsday:
                case xi::Latent::Watersday:
                case xi::Latent::Windsday:
                case xi::Latent::Darksday:
                case xi::Latent::Iceday:
                case xi::Latent::Lightningsday:
                case xi::Latent::Lightsday:
                    return true;
                default:
                    return false;
            }
        case LatentTimeRefresh::Hour:
            return condition == xi::Latent::HourOfDay;
        default:
            return false;
    }
}

} // namespace latenthelpers
