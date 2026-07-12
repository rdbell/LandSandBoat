#pragma once

#include <cstdint>

// Pure primary weaponskill side-effect policy from CCharEntity::OnWeaponSkillFinished.

namespace charweaponskillprimaryhelpers
{

// ActionProcSkillChain pins from enums/action/proc_kind.h.
constexpr std::uint8_t SkillchainNone         = 0;
constexpr std::uint8_t SkillchainGravitation  = 3;
constexpr std::uint8_t SkillchainCompression  = 7;
constexpr std::uint8_t SkillchainRadiance     = 15;

// ShouldNotifyHit mirrors !selfTargetWS && primary && targetIsMob.
constexpr auto ShouldNotifyHit(const bool selfTargetWS, const bool primary, const bool targetIsMob) -> bool
{
    return !selfTargetWS && primary && targetIsMob;
}

// IsNegatedResolution mirrors Miss || Parry.
constexpr auto IsNegatedResolution(const bool isMiss, const bool isParry) -> bool
{
    return isMiss || isParry;
}

// SkillchainWSPointLevel classifies effect into the 1/2/3 WS_POINTS_SKILLCHAIN
// multiplier band used after a non-None skillchain effect:
//   [Compression, Radiance) → 1
//   >= Gravitation (and not in band 1) → 2
//   else → 3
constexpr auto SkillchainWSPointLevel(const std::uint8_t effect) -> std::uint8_t
{
    if (effect >= SkillchainCompression && effect < SkillchainRadiance)
    {
        return 1;
    }
    if (effect >= SkillchainGravitation)
    {
        return 2;
    }
    return 3;
}

// SkillchainWSPointBonus is level * wsPointsSkillchain.
constexpr auto SkillchainWSPointBonus(const std::uint8_t effect, const std::uint8_t wsPointsSkillchain) -> std::uint8_t
{
    return static_cast<std::uint8_t>(SkillchainWSPointLevel(effect) * wsPointsSkillchain);
}

// TotalWSPoints is base + optional skillchain bonus when a non-None effect fired.
constexpr auto TotalWSPoints(const std::uint8_t base,
                             const bool hasSkillchainEffect,
                             const std::uint8_t effect,
                             const std::uint8_t wsPointsSkillchain) -> std::uint8_t
{
    if (!hasSkillchainEffect)
    {
        return base;
    }
    return static_cast<std::uint8_t>(base + SkillchainWSPointBonus(effect, wsPointsSkillchain));
}

// ShouldAwardWSPoints mirrors primary && !negated && difficulty > TooWeak.
constexpr auto ShouldAwardWSPoints(const bool primary, const bool isNegated, const bool aboveTooWeak) -> bool
{
    return primary && !isNegated && aboveTooWeak;
}

} // namespace charweaponskillprimaryhelpers
