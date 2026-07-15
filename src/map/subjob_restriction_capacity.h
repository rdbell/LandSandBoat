#pragma once

#include <cstdint>

// Pure CBattleEntity::GetSJob / GetSLevel restriction policy.
// Parity: internal/subjobrestriction (slice 1661).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::GetSJob   (~1704–1712)
//   CBattleEntity::GetSLevel (~1714–1722)
//
// Host retains StatusEffectContainer and m_sjob / m_slvl storage; helpers take
// injected effect-presence flags and stored job/level only.

namespace subjobrestrictionhelpers
{

// JOBTYPE::JOB_NON (0).
constexpr std::uint8_t JobNone = 0;

// xi::StatusEffect::SjRestriction / xi.effect.SJ_RESTRICTION (157).
constexpr std::uint16_t EffectSjRestriction = 157;

// xi::StatusEffect::Obliviscence / xi.effect.OBLIVISCENCE (260).
constexpr std::uint16_t EffectObliviscence = 260;

// SubjobRestricted mirrors HasStatusEffect({Obliviscence, SjRestriction}).
// Either effect alone restricts the subjob.
constexpr auto SubjobRestricted(const bool hasObliviscence, const bool hasSjRestriction) -> bool
{
    return hasObliviscence || hasSjRestriction;
}

// ResolveSJob mirrors CBattleEntity::GetSJob(ignoreRestriction):
//   if (!ignoreRestriction && restricted) return JOB_NON;
//   return m_sjob;
// restricted is the host-resolved SubjobRestricted result (or equivalent).
constexpr auto ResolveSJob(const std::uint8_t sjob,
                           const bool         ignoreRestriction,
                           const bool         restricted) -> std::uint8_t
{
    if (!ignoreRestriction && restricted)
    {
        return JobNone;
    }
    return sjob;
}

// ResolveSLevel mirrors CBattleEntity::GetSLevel:
//   if (restricted) return 0;
//   return m_slvl;
// LSB has no ignoreRestriction parameter on GetSLevel.
constexpr auto ResolveSLevel(const std::uint8_t slvl, const bool restricted) -> std::uint8_t
{
    if (restricted)
    {
        return 0;
    }
    return slvl;
}

} // namespace subjobrestrictionhelpers
