#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure fishing hook-chance / tidal / moon formulas from fishingutils.
// Parity: internal/fishingutils (hook_chance, patterns, moon_phase; slice 1619).

namespace fishinghookchancehelpers
{

// FISHINGBAITTYPE_LURE
constexpr std::uint8_t BaitTypeLure = 1;

// Flag bits
constexpr std::uint32_t BaitFlagShellfishAffinity = 0x40;
constexpr std::uint32_t FishFlagShellfish         = 0x01;

inline auto Clamp01(const float v) -> float
{
    if (v < 0.0f)
    {
        return 0.0f;
    }
    if (v > 1.0f)
    {
        return 1.0f;
    }
    return v;
}

inline auto CosClampPattern(const float scale, const float phase, const std::uint8_t x) -> float
{
    const auto arg = scale * static_cast<float>(x) + phase;
    const auto v   = 0.5f * static_cast<float>(std::cos(static_cast<double>(arg))) + 0.5f;
    return Clamp01(v);
}

inline auto HourPattern1(const std::uint8_t x) -> float { return CosClampPattern(0.82f, 0.16f, x); }
inline auto HourPattern2(const std::uint8_t x) -> float { return CosClampPattern(0.60f, 3.50f, x); }
inline auto HourPattern3(const std::uint8_t x) -> float { return CosClampPattern(0.53f, 0.00f, x); }
inline auto HourPattern4(const std::uint8_t x) -> float { return CosClampPattern(0.23f, 3.53f, x); }
inline auto MoonPattern1(const std::uint8_t x) -> float { return CosClampPattern(1.75f, 0.10f, x); }
inline auto MoonPattern2(const std::uint8_t x) -> float { return CosClampPattern(1.75f, 3.30f, x); }
inline auto MoonPattern3(const std::uint8_t x) -> float
{
    return Clamp01(1.0f - static_cast<float>(static_cast<int>(x) / 7));
}
inline auto MoonPattern4(const std::uint8_t x) -> float { return CosClampPattern(0.90f, 3.14f, x); }
inline auto MoonPattern5(const std::uint8_t x) -> float { return CosClampPattern(0.90f, 0.00f, x); }
inline auto MonthPattern1(const std::uint8_t x) -> float { return CosClampPattern(0.40f, -1.60f, x); }
inline auto MonthPattern2(const std::uint8_t x) -> float { return CosClampPattern(0.60f, -1.00f, x); }
inline auto MonthPattern3(const std::uint8_t x) -> float { return CosClampPattern(0.50f, 3.05f, x); }
inline auto MonthPattern4(const std::uint8_t x) -> float { return CosClampPattern(1.04f, 0.00f, x); }
inline auto MonthPattern5(const std::uint8_t x) -> float { return CosClampPattern(0.40f, 3.50f, x); }
inline auto MonthPattern6(const std::uint8_t x) -> float { return CosClampPattern(0.90f, -2.00f, x); }
inline auto MonthPattern7(const std::uint8_t x) -> float { return CosClampPattern(0.49f, 1.63f, x); }
inline auto MonthPattern8(const std::uint8_t x) -> float { return CosClampPattern(1.04f, -2.60f, x); }
inline auto MonthPattern9(const std::uint8_t x) -> float { return CosClampPattern(0.49f, -1.25f, x); }
inline auto MonthPattern10(const std::uint8_t x) -> float { return CosClampPattern(0.50f, 0.53f, x); }

// Pure GetMonthlyTidalInfluence with month index inject (vanadiel month - 1).
inline auto GetMonthlyTidalInfluence(const std::uint8_t monthPattern, const std::uint8_t monthIndex) -> float
{
    auto modifier = 0.5f;
    switch (monthPattern)
    {
        case 1:
            modifier = MonthPattern1(monthIndex);
            break;
        case 2:
            modifier = MonthPattern2(monthIndex);
            break;
        case 3:
            modifier = MonthPattern3(monthIndex);
            break;
        case 4:
            modifier = MonthPattern4(monthIndex);
            break;
        case 5:
            modifier = MonthPattern5(monthIndex);
            break;
        case 6:
            modifier = MonthPattern6(monthIndex);
            break;
        case 7:
            modifier = MonthPattern7(monthIndex);
            break;
        case 8:
            modifier = MonthPattern8(monthIndex);
            break;
        case 9:
            modifier = MonthPattern9(monthIndex);
            break;
        case 10:
            modifier = MonthPattern10(monthIndex);
            break;
        default:
            break;
    }
    return modifier + 0.25f;
}

// Pure GetHourlyModifier with hour inject.
inline auto GetHourlyModifier(const std::uint8_t hourPattern, const std::uint8_t hour) -> float
{
    auto modifier = 0.5f;
    switch (hourPattern)
    {
        case 1:
            modifier = HourPattern1(hour);
            break;
        case 2:
            if (hour != 5 && hour != 17)
            {
                modifier = 1.0f;
            }
            break;
        case 3:
            if (hour == 5 || hour == 17)
            {
                modifier = 1.0f;
            }
            break;
        case 4:
            if (hour > 19 || hour < 4)
            {
                modifier = 1.0f;
            }
            break;
        case 5:
            modifier = HourPattern2(hour);
            break;
        case 6:
            modifier = HourPattern3(hour);
            break;
        case 7:
            modifier = HourPattern4(hour);
            break;
        default:
            break;
    }
    return modifier + 0.25f;
}

// Pure GetMoonModifier with discrete moon phase inject.
// Case 5 uses MoonPattern4 (LSB quirk).
inline auto GetMoonModifier(const std::uint8_t moonPattern, const std::uint8_t moonPhase) -> float
{
    auto modifier = 1.0f;
    switch (moonPattern)
    {
        case 1:
            modifier = MoonPattern1(moonPhase);
            break;
        case 2:
            modifier = MoonPattern2(moonPhase);
            break;
        case 3:
            modifier = MoonPattern3(moonPhase);
            break;
        case 4:
            modifier = MoonPattern4(moonPhase);
            break;
        case 5:
            modifier = MoonPattern4(moonPhase);
            break;
        default:
            break;
    }
    return modifier + 0.25f;
}

// Pure GetMoonPhase classifier with phase percent + direction injects.
inline auto GetMoonPhase(const std::uint8_t phase, const std::uint8_t direction) -> std::uint8_t
{
    if (phase <= 5 || (phase <= 10 && direction == 1))
    {
        return 0; // NEW
    }
    if (phase >= 7 && phase <= 38 && direction == 2)
    {
        return 1; // WAXING_CRESCENT
    }
    if (phase >= 40 && phase <= 55 && direction == 2)
    {
        return 2; // FIRST_QUARTER
    }
    if (phase >= 57 && phase <= 88 && direction == 2)
    {
        return 3; // WAXING_GIBBOUS
    }
    if (phase >= 95 || (phase >= 90 && direction == 2))
    {
        return 4; // FULL
    }
    if (phase >= 62 && phase <= 93 && direction == 1)
    {
        return 5; // WANING_GIBBOUS
    }
    if (phase >= 45 && phase <= 60 && direction == 1)
    {
        return 6; // LAST_QUARTER
    }
    if (phase >= 12 && phase < 43 && direction == 1)
    {
        return 7; // WANING_CRESCENT
    }
    return 0;
}

// Pure GetLuckyMoonModifier from discrete moon phase.
inline auto GetLuckyMoonModifierFromPhase(const std::uint8_t moonPhase) -> std::uint8_t
{
    return static_cast<std::uint8_t>(1 + static_cast<std::uint8_t>(std::floor(MoonPattern1(moonPhase) * 3.0f)));
}

// Pure CalculateHookChance. monthMod/hourMod/moonMod are Get*Modifier results
// (already +0.25). Host multiplies hour*2 and moon*3 inside, matching Go pure.
inline auto CalculateHookChance(const std::uint8_t fishingSkill, const std::uint8_t fishMaxSkill, const std::uint8_t fishSizeType, const std::uint16_t fishRarity,
                                const bool fishShellfish, const std::uint8_t baitPower, const bool baitIsLure, const bool baitShellfishAffinity,
                                const bool rodLegendary, const std::uint8_t rodSizeType, const float monthMod, const float hourMod, const float moonMod)
    -> std::uint16_t
{
    const auto monthModifier = monthMod;
    const auto hourModifier  = hourMod * 2.0f;
    const auto moonModifier  = moonMod * 3.0f;
    auto       modifier      = (moonModifier + hourModifier + monthModifier) / 3.0f;
    if (modifier < 0.0f)
    {
        modifier = 0.0f;
    }
    auto hookChance = static_cast<std::uint16_t>(std::floor(25.0f * modifier));

    switch (baitPower)
    {
        case 1:
            hookChance = static_cast<std::uint16_t>(hookChance + (baitIsLure ? 30 : 35));
            break;
        case 2:
            hookChance = static_cast<std::uint16_t>(hookChance + (baitIsLure ? 60 : 65));
            break;
        case 3:
            hookChance = static_cast<std::uint16_t>(hookChance + (baitIsLure ? 75 : 80));
            break;
        default:
            break;
    }

    if (fishMaxSkill > fishingSkill)
    {
        const auto pen = static_cast<std::uint16_t>(std::floor(static_cast<float>(static_cast<int>(fishMaxSkill) - static_cast<int>(fishingSkill)) * 0.25f));
        hookChance     = static_cast<std::uint16_t>(hookChance - std::min(pen, hookChance));
    }

    if (static_cast<int>(fishingSkill) - 10 > static_cast<int>(fishMaxSkill))
    {
        const auto pen =
            static_cast<std::uint16_t>(std::floor(static_cast<float>(static_cast<int>(fishingSkill) - 10 - static_cast<int>(fishMaxSkill)) * 0.15f));
        hookChance = static_cast<std::uint16_t>(hookChance - std::min(pen, hookChance));
    }

    if (!rodLegendary)
    {
        if (fishSizeType < rodSizeType)
        {
            const auto pen = static_cast<std::uint16_t>(3);
            hookChance     = static_cast<std::uint16_t>(hookChance - std::min(pen, hookChance));
        }
        else if (rodSizeType < fishSizeType)
        {
            const auto pen = static_cast<std::uint16_t>(5);
            hookChance     = static_cast<std::uint16_t>(hookChance - std::min(pen, hookChance));
        }
    }

    if (baitShellfishAffinity && fishShellfish)
    {
        hookChance = static_cast<std::uint16_t>(hookChance + 50);
    }

    if (fishRarity < 1000)
    {
        const auto multiplier = static_cast<float>(fishRarity) / 1000.0f;
        hookChance            = static_cast<std::uint16_t>(std::floor(static_cast<float>(hookChance) * multiplier));
    }

    return std::clamp<std::uint16_t>(hookChance, 20, 120);
}

} // namespace fishinghookchancehelpers
