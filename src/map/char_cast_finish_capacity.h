#pragma once

#include <cstdint>

// Pure OnCastFinished gates for Trick Attack eligibility, blue skillchain
// application, and post-blue-SC TP residual from CCharEntity.

namespace charcastfinishhelpers
{

// ShouldResolveTrickAttack mirrors TA status + blue + dealsDamage.
constexpr auto ShouldResolveTrickAttack(const bool hasTrickAttack,
                                        const bool isBlueSpell,
                                        const bool dealsDamage) -> bool
{
    return hasTrickAttack && isBlueSpell && dealsDamage;
}

// ShouldApplyBlueSkillchain mirrors the blue SC application gate before
// GetSkillChainEffect.
constexpr auto ShouldApplyBlueSkillchain(const std::int32_t resultParam,
                                         const bool dealsDamage,
                                         const bool isBlueSpell,
                                         const bool hasChainAffinityOrAzureLore,
                                         const bool hasPrimarySkillchain) -> bool
{
    return resultParam > 0 && dealsDamage && isBlueSpell && hasChainAffinityOrAzureLore && hasPrimarySkillchain;
}

// RemainingTPAfterBlueSkillchain mirrors Sekkanoki/Meikyo residual vs full clear.
constexpr auto RemainingTPAfterBlueSkillchain(const std::int16_t currentTP, const bool hasSekkanokiOrMeikyo) -> std::int16_t
{
    if (hasSekkanokiOrMeikyo)
    {
        return currentTP > 1000 ? static_cast<std::int16_t>(currentTP - 1000) : 0;
    }
    return 0;
}

// ShouldApplyImmanenceSkillchain mirrors target alive + non-negative param +
// damaging black magic under Immanence (family mapping remains host-side).
constexpr auto ShouldApplyImmanenceSkillchain(const bool targetAlive,
                                             const std::int32_t resultParam,
                                             const bool dealsDamage,
                                             const bool isBlackSpell,
                                             const bool hasImmanence) -> bool
{
    return targetAlive && resultParam >= 0 && dealsDamage && isBlackSpell && hasImmanence;
}

} // namespace charcastfinishhelpers
