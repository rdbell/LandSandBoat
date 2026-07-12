#pragma once

#include <cstdint>

// Pure AddExperiencePoints gates and messaging classification from charutils.

namespace expawardhelpers
{

// ShouldRejectDead mirrors isDead && !expFromRaise.
constexpr auto ShouldRejectDead(const bool isDead, const bool expFromRaise) -> bool
{
    return isDead && !expFromRaise;
}

// ShouldApplyExpRate mirrors !expFromRaise && !fromScripts.
constexpr auto ShouldApplyExpRate(const bool expFromRaise, const bool fromScripts) -> bool
{
    return !expFromRaise && !fromScripts;
}

// IsLimitModeFromMerit mirrors MeritMode && job > 74 && !expFromRaise.
constexpr auto IsLimitModeFromMerit(const bool meritMode, const std::uint8_t jobLevel, const bool expFromRaise) -> bool
{
    return meritMode && jobLevel > 74 && !expFromRaise;
}

// IsLimitModeFromCap mirrors job > 74 && job >= genkai && exp == next-1.
constexpr auto IsLimitModeFromCap(const std::uint8_t jobLevel,
                                  const std::uint8_t genkai,
                                  const std::uint16_t currentExp,
                                  const std::uint32_t nextLevelExp) -> bool
{
    return jobLevel > 74 && jobLevel >= genkai && currentExp == static_cast<std::uint16_t>(nextLevelExp - 1);
}

// ShouldShowExpMessage mirrors !expFromRaise && exp > 0.
constexpr auto ShouldShowExpMessage(const bool expFromRaise, const std::uint32_t exp) -> bool
{
    return !expFromRaise && exp > 0;
}

// ShouldUseChainMessage mirrors mobCheck >= EvenMatch && isexpchain.
constexpr auto ShouldUseChainMessage(const bool evenMatchOrAbove, const bool isExpChain) -> bool
{
    return evenMatchOrAbove && isExpChain;
}

// Message class for battle-message2 selection.
enum class ExpMessage : std::uint8_t
{
    ExpGained = 0,
    ExpChain,
    LimitGained,
    LimitChain,
};

// SelectMessage picks the MsgBasic class for EXP/limit awards.
constexpr auto SelectMessage(const bool onLimitMode, const bool useChainMessage, const bool chainNumberNonZero) -> ExpMessage
{
    if (useChainMessage && chainNumberNonZero)
    {
        return onLimitMode ? ExpMessage::LimitChain : ExpMessage::ExpChain;
    }
    if (useChainMessage)
    {
        // chain active path with chainNumber == 0 still uses gained messages
        return onLimitMode ? ExpMessage::LimitGained : ExpMessage::ExpGained;
    }
    return onLimitMode ? ExpMessage::LimitGained : ExpMessage::ExpGained;
}

// ShouldIncrementChainNumber mirrors useChainMessage path that increments.
constexpr auto ShouldIncrementChainNumber(const bool useChainMessage) -> bool
{
    return useChainMessage;
}

// ShouldLevelUp mirrors (currentExp + exp) >= nextLevel && !onLimitMode.
constexpr auto ShouldLevelUp(const std::uint32_t currentExp, const std::uint32_t exp, const std::uint32_t nextLevelExp, const bool onLimitMode) -> bool
{
    return !onLimitMode && (currentExp + exp) >= nextLevelExp;
}

// IsAtGenkaiCap mirrors job >= genkai for field de-level / cap handling.
constexpr auto IsAtGenkaiCap(const std::uint8_t jobLevel, const std::uint8_t genkai) -> bool
{
    return jobLevel >= genkai;
}

// CapExpAtNextMinusOne is nextLevelExp - 1.
constexpr auto CapExpAtNextMinusOne(const std::uint32_t nextLevelExp) -> std::uint32_t
{
    return nextLevelExp - 1;
}

// PostLevelExp residual after subtracting next-level cost, capped at nextNext-1.
constexpr auto PostLevelResidualExp(const std::uint32_t expAfterSubtract, const std::uint32_t nextNextLevelExp) -> std::uint32_t
{
    if (expAfterSubtract >= nextNextLevelExp)
    {
        return nextNextLevelExp - 1;
    }
    return expAfterSubtract;
}

// ShouldApplyLevelToEntity mirrors m_LevelRestriction == 0 || restriction > newMLevel.
constexpr auto ShouldApplyLevelToEntity(const std::uint8_t levelRestriction, const std::uint8_t newMainLevel) -> bool
{
    return levelRestriction == 0 || levelRestriction > newMainLevel;
}

// ShouldShowLevelUpAnimation mirrors !expFromRaise after a real level-up.
constexpr auto ShouldShowLevelUpAnimation(const bool expFromRaise) -> bool
{
    return !expFromRaise;
}

} // namespace expawardhelpers
