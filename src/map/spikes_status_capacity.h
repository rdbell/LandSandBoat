#pragma once

#include <cstdint>

// Pure HandleSpikesStatusEffect chance and status selection.

namespace spikesstatushelpers
{

constexpr std::uint8_t ReactIceSpikes   = 2;
constexpr std::uint8_t ReactCurseSpikes = 4;
constexpr std::uint8_t ReactShockSpikes = 5;

constexpr int IceSpikeProcBase   = 20;
constexpr int ShockSpikeProcBase = 30;

enum class SpikesStatusAction : std::uint8_t
{
    None = 0,
    ApplyCurse,
    ApplyParalysis,
    ApplyStun,
};

constexpr auto SpikesStatusLvlDiff(const bool hasDefender, const std::uint8_t defenderLevel, const std::uint8_t attackerLevel) -> int
{
    if (!hasDefender)
    {
        return 0;
    }
    int diff = static_cast<int>(defenderLevel) - static_cast<int>(attackerLevel);
    if (diff < -5)
    {
        diff = -5;
    }
    if (diff > 5)
    {
        diff = 5;
    }
    return diff * 2;
}

constexpr auto ResolveSpikesStatusEffect(const std::uint8_t spikesEffect,
                                         const bool         hasDefender,
                                         const std::uint8_t defenderLevel,
                                         const std::uint8_t attackerLevel,
                                         const bool         hasCurseI,
                                         const bool         hasParalysis,
                                         const bool         hasStun,
                                         const int          roll) -> SpikesStatusAction
{
    const int lvlDiff = SpikesStatusLvlDiff(hasDefender, defenderLevel, attackerLevel);
    switch (spikesEffect)
    {
        case ReactCurseSpikes:
            if (!hasCurseI)
            {
                return SpikesStatusAction::ApplyCurse;
            }
            break;
        case ReactIceSpikes:
            if (roll < IceSpikeProcBase + lvlDiff && !hasParalysis)
            {
                return SpikesStatusAction::ApplyParalysis;
            }
            break;
        case ReactShockSpikes:
            if (roll < ShockSpikeProcBase + lvlDiff && !hasStun)
            {
                return SpikesStatusAction::ApplyStun;
            }
            break;
        default:
            break;
    }
    return SpikesStatusAction::None;
}

} // namespace spikesstatushelpers
