#pragma once

#include <algorithm>
#include <cstdint>

// Pure CalculateWyvernStats policy (level, combat fixed mods, JP HP).
// Weapon damage reuses petweapondamagehelpers::WyvernWeaponDamage.
// Parity: internal/petutils wyvern_stats.go (slice 1605).

namespace wyvernstatshelpers
{

// Main weapon delay / base delay pin.
constexpr std::uint16_t WeaponDelay = 320;

// Innate Mod::DMG (−40%) and Mod::SUBTLE_BLOW.
constexpr std::int16_t DamageTaken = -4000;
constexpr std::int16_t SubtleBlow  = 40;

// MOBMOD_CAN_PARRY value.
constexpr std::uint16_t CanParry = 1;

// GetMaxSkill level arg: mLvl > 99 ? 99 : mLvl.
constexpr auto SkillCapLevel(const std::uint8_t mLvl) -> std::uint8_t
{
    return mLvl > 99 ? 99 : mLvl;
}

// clamp(mainhandItemLevel - 99, 0, 20).
constexpr auto ILvlBonus(const std::uint8_t mainhandItemLevel) -> std::uint8_t
{
    return static_cast<std::uint8_t>(std::clamp(static_cast<int>(mainhandItemLevel) - 99, 0, 20));
}

// Pet main level: master mLvl + iLvl bonus + WYVERN_LVL_BONUS mod.
constexpr auto PetMainLevel(const std::uint8_t masterMLvl, const std::uint8_t iLvlBonus, const std::int16_t wyvernLvlBonus) -> std::uint8_t
{
    const auto sum = static_cast<int>(masterMLvl) + static_cast<int>(iLvlBonus) + static_cast<int>(wyvernLvlBonus);
    if (sum < 0)
    {
        return 0;
    }
    if (sum > 255)
    {
        return 255;
    }
    return static_cast<std::uint8_t>(sum);
}

// JP_WYVERN_MAX_HP_BONUS × 10.
constexpr auto MaxHPJobPointBonus(const std::uint8_t jpValue) -> std::int16_t
{
    return static_cast<std::int16_t>(jpValue * 10);
}

} // namespace wyvernstatshelpers
