#pragma once

#include <algorithm>
#include <cstdint>

// Pure DelExperiencePoints policy from charutils.

namespace explosshelpers
{

// IsRetainPercentValid mirrors 0 <= retainPercent <= 1.
inline auto IsRetainPercentValid(const float retainPercent) -> bool
{
    return retainPercent >= 0.0f && retainPercent <= 1.0f;
}

// IsExpLossLevelSettingValid mirrors 1 <= setting <= 99.
constexpr auto IsExpLossLevelSettingValid(const std::uint8_t setting) -> bool
{
    return setting >= 1 && setting <= 99;
}

// ShouldSkipByLevel mirrors mainLevel < expLossLevel && forcedXpLoss == 0.
constexpr auto ShouldSkipByLevel(const std::uint8_t mainLevel, const std::uint8_t expLossLevel, const std::uint16_t forcedXpLoss) -> bool
{
    return mainLevel < expLossLevel && forcedXpLoss == 0;
}

// EffectiveLossLevel mirrors restriction != 0 && restriction < mLevel ? restriction : mLevel.
constexpr auto EffectiveLossLevel(const std::uint8_t mainLevel, const std::uint8_t levelRestriction) -> std::uint8_t
{
    if (levelRestriction != 0 && levelRestriction < mainLevel)
    {
        return levelRestriction;
    }
    return mainLevel;
}

// BaseLossAmount mirrors mLevel <= 67 ? (next*8)/100 : 2400.
constexpr auto BaseLossAmount(const std::uint8_t mLevel, const std::uint32_t nextLevelExp) -> std::uint16_t
{
    if (mLevel <= 67)
    {
        return static_cast<std::uint16_t>((nextLevelExp * 8) / 100);
    }
    return 2400;
}

// ApplyRetentionAndRate mirrors (base * (1-retain)) * lossRate when not forced.
inline auto ApplyRetentionAndRate(const std::uint16_t baseLoss, const float retainPercent, const float lossRate) -> std::uint16_t
{
    auto exploss = static_cast<std::uint16_t>(baseLoss * (1.0f - retainPercent));
    exploss      = static_cast<std::uint16_t>(exploss * lossRate);
    return exploss;
}

// ResolveLossAmount prefers forcedXpLoss when non-zero.
inline auto ResolveLossAmount(const std::uint16_t baseLoss,
                              const std::uint16_t forcedXpLoss,
                              const float retainPercent,
                              const float lossRate) -> std::uint16_t
{
    if (forcedXpLoss > 0)
    {
        return forcedXpLoss;
    }
    return ApplyRetentionAndRate(baseLoss, retainPercent, lossRate);
}

// ShouldDelevel mirrors currentExp < exploss && jobLevel > 1.
constexpr auto ShouldDelevel(const std::uint16_t currentExp, const std::uint16_t exploss, const std::uint8_t jobLevel) -> bool
{
    return currentExp < exploss && jobLevel > 1;
}

// ShouldZeroExpAtLevel1 mirrors currentExp < exploss && jobLevel <= 1.
constexpr auto ShouldZeroExpAtLevel1(const std::uint16_t currentExp, const std::uint16_t exploss, const std::uint8_t jobLevel) -> bool
{
    return currentExp < exploss && jobLevel <= 1;
}

// DelevelResidualExp mirrors max(0, lowerLevelMaxExp - (exploss - currentExp)).
constexpr auto DelevelResidualExp(const std::int32_t lowerLevelMaxExp, const std::uint16_t exploss, const std::uint16_t currentExp) -> std::int32_t
{
    const auto remaining = static_cast<std::int32_t>(exploss) - static_cast<std::int32_t>(currentExp);
    const auto residual  = lowerLevelMaxExp - remaining;
    return residual > 0 ? residual : 0;
}

// ShouldApplyDelevelToEntity mirrors restriction == 0 || newJob < restriction.
constexpr auto ShouldApplyDelevelToEntity(const std::uint8_t levelRestriction, const std::uint8_t newJobLevel) -> bool
{
    return levelRestriction == 0 || newJobLevel < levelRestriction;
}

// RemainingExpAfterLoss mirrors currentExp - exploss when not deleveling.
constexpr auto RemainingExpAfterLoss(const std::uint16_t currentExp, const std::uint16_t exploss) -> std::uint16_t
{
    return static_cast<std::uint16_t>(currentExp - exploss);
}

} // namespace explosshelpers
