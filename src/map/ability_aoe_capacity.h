#pragma once

#include <cstdint>

// Pure xi.combat.abilityAoE.calculateTypeAndRadius.
// Parity: internal/abilityaoe (slice 0846); production wire slice 1581.

namespace abilityaoehelpers
{

constexpr std::uint16_t AbilityLiement      = 373;
constexpr std::uint16_t AbilityHealingWaltz = 194;
constexpr std::uint16_t RecastPhantomRoll   = 193;
constexpr std::uint16_t RecastDoubleUp      = 194;

constexpr std::uint8_t TypeNone  = 0;
constexpr std::uint8_t TypeRound = 1;

constexpr int OverrideRadius = 10;

struct Result
{
    std::uint8_t type{};
    int          radius{};
};

// calculateTypeAndRadius pure.
constexpr auto TypeAndRadius(const std::uint16_t abilityID,
                             const std::uint16_t recastID,
                             const std::uint8_t  baseType,
                             const int           baseRadius,
                             const int           liementExtendsToArea,
                             const bool          hasContradance,
                             const int           rollRange) -> Result
{
    if (abilityID == AbilityLiement && liementExtendsToArea > 0)
    {
        return Result{ TypeRound, OverrideRadius };
    }
    if (abilityID == AbilityHealingWaltz && hasContradance)
    {
        return Result{ TypeRound, OverrideRadius };
    }
    if (recastID == RecastPhantomRoll || recastID == RecastDoubleUp)
    {
        return Result{ TypeRound, baseRadius + rollRange };
    }
    return Result{ baseType, baseRadius };
}

} // namespace abilityaoehelpers
