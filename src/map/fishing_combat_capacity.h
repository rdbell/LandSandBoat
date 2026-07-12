#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure fishing combat formulas from fishingutils (stamina, attack, heal, lucky timing).
// Parity: internal/fishingutils (stamina, attack_heal, lucky_timing; slice 1616 production wire).

namespace fishingcombathelpers
{

// BAITFLAG_GOLD_ARROW_BONUS (fishingutils.h).
constexpr std::uint32_t BaitFlagGoldArrowBonus = 0x10;

// Rod item IDs used by lucky-timing legendary rod bonus.
constexpr std::uint16_t RodIDLuShang = 17386;
constexpr std::uint16_t RodIDEbisu   = 17011;

// FISHINGGEAR item IDs.
constexpr std::uint16_t GearFishermansTunica = 13808;
constexpr std::uint16_t GearAnglersTunica    = 13809;
constexpr std::uint16_t GearFishermansApron  = 14400;
constexpr std::uint16_t GearFishermansSmock  = 11337;
constexpr std::uint16_t GearFishermansGloves = 14070;
constexpr std::uint16_t GearAnglersGloves    = 14071;
constexpr std::uint16_t GearFishermansHose   = 14292;
constexpr std::uint16_t GearAnglersHose      = 14293;
constexpr std::uint16_t GearFishermansBoots  = 14171;
constexpr std::uint16_t GearAnglersBoots     = 14172;
constexpr std::uint16_t GearWaders           = 14195;

// CalculateStamina with injected roll in [95, 104] (xirand 95..105 exclusive end).
inline auto CalculateStamina(const int skill, const std::uint8_t count, const int roll) -> std::uint16_t
{
    const auto multiplier = 1.0f + (0.1f * static_cast<float>(static_cast<int>(count) - 1));
    const auto modSkill   = static_cast<int>(std::floor(multiplier * static_cast<float>(skill)));
    // Integer product matches LSB floor of int product.
    return static_cast<std::uint16_t>(roll * ((modSkill + 36) / 2));
}

inline auto CalculateAttack(const bool legendary, const std::uint8_t difficulty, const std::uint8_t fishAttack, const std::uint8_t lgdBonusAtk) -> std::uint16_t
{
    const auto bonusAdd = legendary ? lgdBonusAtk : static_cast<std::uint8_t>(0);
    const auto ratio    = (static_cast<float>(fishAttack) + static_cast<float>(bonusAdd)) / 100.0f;
    return static_cast<std::uint16_t>(std::floor(static_cast<float>(difficulty) * ratio * 20.0f));
}

// Floor first, then *10 — matches LSB cast order.
inline auto CalculateHeal(const bool legendary, const std::uint8_t difficulty, const std::uint8_t fishAttack, const std::uint8_t lgdBonusAtk,
                          const std::uint8_t fishRecovery) -> std::uint16_t
{
    const auto attack  = CalculateAttack(legendary, difficulty, fishAttack, lgdBonusAtk);
    const auto inner   = (static_cast<float>(attack) / 20.0f) * (static_cast<float>(fishRecovery) / 100.0f);
    const auto floored = static_cast<std::uint16_t>(std::floor(inner));
    return static_cast<std::uint16_t>(static_cast<float>(floored) * 10.0f);
}

// Gear slots are uint32 on fishing_gear_t.
inline auto LuckyTimingGearBonus(const std::uint32_t body, const std::uint32_t hands, const std::uint32_t legs, const std::uint32_t feet) -> float
{
    auto bonus = 0.0f;
    switch (body)
    {
        case GearFishermansTunica:
            bonus += 0.5f;
            break;
        case GearAnglersTunica:
            bonus += 1.0f;
            break;
        case GearFishermansApron:
        case GearFishermansSmock:
            bonus += 3.0f;
            break;
        default:
            break;
    }
    switch (hands)
    {
        case GearFishermansGloves:
            bonus += 0.5f;
            break;
        case GearAnglersGloves:
            bonus += 1.0f;
            break;
        default:
            break;
    }
    switch (legs)
    {
        case GearFishermansHose:
            bonus += 0.5f;
            break;
        case GearAnglersHose:
            bonus += 1.0f;
            break;
        default:
            break;
    }
    switch (feet)
    {
        case GearFishermansBoots:
            bonus += 0.5f;
            break;
        case GearAnglersBoots:
            bonus += 1.0f;
            break;
        case GearWaders:
            bonus += 2.0f;
            break;
        default:
            break;
    }
    return bonus;
}

// Pure CalculateLuckyTiming with host injects (gear bonus pre-summed, moon/hour/roll inject).
inline auto CalculateLuckyTiming(const std::uint8_t fishingSkill, const std::uint8_t catchSkill, const std::uint8_t sizeType, const std::uint16_t rodID,
                                 const std::uint8_t rodSizeType, const bool rodLegendary, const bool catchLegendary, const std::uint32_t baitFlags,
                                 const float gearBonus, const std::uint8_t moonModifier, const std::uint8_t gameHour, const std::uint8_t moonRoll) -> std::uint8_t
{
    auto luckyTiming = 10;
    auto penalty     = 0.0f;
    auto bonus       = 0.0f;

    if (static_cast<int>(catchSkill) > static_cast<int>(fishingSkill) + 7)
    {
        const auto diff = static_cast<int>(catchSkill) - (static_cast<int>(fishingSkill) + 7);
        penalty += static_cast<float>(static_cast<std::uint8_t>(diff));
    }
    else if (static_cast<int>(fishingSkill) + 10 > static_cast<int>(catchSkill))
    {
        const auto div = (static_cast<int>(fishingSkill) + 10 - static_cast<int>(catchSkill)) / 20;
        bonus += static_cast<float>(static_cast<std::uint8_t>(div));
    }

    bonus += static_cast<float>(static_cast<int>(moonModifier) * 5 + static_cast<int>(moonModifier) * static_cast<int>(moonRoll));

    if (gameHour == 6 || gameHour == 7 || (gameHour >= 16 && gameHour <= 18))
    {
        bonus += 9.0f;
    }
    else if (gameHour >= 8 && gameHour <= 15)
    {
        bonus += 3.0f;
    }
    else
    {
        bonus += 6.0f;
    }

    if (catchLegendary && rodLegendary)
    {
        if (rodID == RodIDLuShang)
        {
            bonus += 6.0f;
        }
        else if (rodID == RodIDEbisu)
        {
            bonus += 8.0f;
        }
        else if (rodSizeType == sizeType)
        {
            bonus += 4.0f;
        }
        else if (rodSizeType > sizeType)
        {
            bonus += 2.0f;
        }
    }

    bonus += gearBonus;

    if (baitFlags & BaitFlagGoldArrowBonus)
    {
        bonus *= 1.25f;
    }

    luckyTiming += static_cast<int>(static_cast<std::uint8_t>(std::floor(bonus)));

    auto pen = penalty;
    if (pen > static_cast<float>(luckyTiming))
    {
        pen = static_cast<float>(luckyTiming);
    }
    luckyTiming -= static_cast<int>(static_cast<std::uint8_t>(std::floor(pen)));

    return std::max<std::uint8_t>(5, static_cast<std::uint8_t>(luckyTiming));
}

} // namespace fishingcombathelpers
