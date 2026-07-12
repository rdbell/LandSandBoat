#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure mobutils::CalculateBaseMobHP / CalculateSubjobHP.
// Parity: internal/mobutils mob_hp.go (slice 1600).

namespace mobhphelpers
{

// Main-job HP from grade scales and main level.
inline auto CalculateBaseMobHP(const std::uint8_t mLvl, const std::uint8_t baseHP, const std::uint8_t jobScale, const std::uint8_t scaleXHP) -> std::uint32_t
{
    // HP formula has multiple parts based on level ranges:
    // Levels 1-5: Base HP + scaling per level
    // Levels 5-30: Additional scaling with conditional multiplier
    // Levels 30+: Increased scaling with special modifiers
    if (mLvl == 0)
    {
        return 0;
    }

    const std::uint8_t level5Scaling  = std::min(mLvl, static_cast<std::uint8_t>(5));
    const std::uint8_t level30Scaling = std::min(mLvl, static_cast<std::uint8_t>(30));

    std::uint32_t hp = baseHP + (level5Scaling - 1) * (jobScale + 5);

    // Additional bonuses based on scaling thresholds
    std::uint32_t riBonus = 0;
    switch (level5Scaling)
    {
        case 0:
        case 1:
        case 2:
            riBonus = 0;
            break;
        case 3:
            riBonus = 3;
            break;
        case 4:
            riBonus = 7;
            break;
        default: // 5
            riBonus = 14;
            break;
    }

    hp += riBonus;

    if (mLvl > 5)
    {
        const std::uint32_t level5Bonus = (level30Scaling - 5) * (2 * jobScale + level30Scaling + 6) / 2;
        hp += level5Bonus;
    }

    if (mLvl > 30)
    {
        const std::uint32_t level30Bonus = (mLvl - 30) * (63 + scaleXHP) + (mLvl - 31) * (jobScale + 6);
        hp += level30Bonus;
    }

    return hp;
}

// Subjob HP contribution scaled by main-level bands.
inline auto CalculateSubjobHP(const std::uint8_t mLvl, const std::uint8_t sjJobScale, const std::uint8_t sjScaleXHP) -> std::uint32_t
{
    // Subjob HP contribution varies by main job level:
    // 50+   = 100% of subjob stats
    // 40-49 = 75% of subjob stats
    // 31-39 = 50% of subjob stats
    // 25-30 = 25% of subjob stats
    // 1-24  = 0% of subjob stats
    int sjScale = 0;
    if (mLvl > 49)
    {
        sjScale = mLvl;
    }
    else if (mLvl > 39)
    {
        sjScale = (mLvl * 3) / 4;
    }
    else if (mLvl > 30)
    {
        sjScale = mLvl / 2;
    }
    else if (mLvl > 24)
    {
        sjScale = mLvl / 4;
    }

    const double sjHp =
        sjJobScale * std::max(sjScale - 1, 0) +
        (0.5 + 0.5 * sjScaleXHP) * std::max(sjScale - 10, 0) +
        std::max(sjScale - 30, 0) +
        std::max(sjScale - 50, 0) +
        std::max(sjScale - 70, 0);

    return static_cast<std::uint32_t>(std::ceil(sjHp / 2.0));
}

} // namespace mobhphelpers
