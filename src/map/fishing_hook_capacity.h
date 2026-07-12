#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure fishing hook/sense residual formulas from fishingutils.
// Parity: internal/fishingutils (hook_time, critical_bite, fish_sense, big_fish,
// mob_modifiers, weather/live bait; slice 1618 production wire).

namespace fishinghookhelpers
{

// Rod item IDs.
constexpr std::uint16_t RodIDEbisu = 17011;

// Live bait IDs (LIVEFISHINGBAIT).
constexpr std::uint16_t BaitIDDrillCalamary = 17006;
constexpr std::uint16_t BaitIDDwarfPugil    = 17007;

// Weather enum values used by GetWeatherModifier (enums/weather.h).
constexpr std::uint16_t WeatherRain   = 6;
constexpr std::uint16_t WeatherSquall = 7;

// MOONPHASES (new/full used by GetHookTimeWait).
constexpr std::uint8_t MoonPhaseNew  = 0;
constexpr std::uint8_t MoonPhaseFull = 4;

// RODFLAG bits.
constexpr std::uint32_t RodFlagSmallPenalty   = 0x01;
constexpr std::uint32_t RodFlagLargePenalty   = 0x02;
constexpr std::uint32_t RodFlagLegendaryBonus = 0x04;

// FISHINGLEGENDARY bits.
constexpr std::uint32_t LegendaryFlagHalfTime           = 0x01;
constexpr std::uint32_t LegendaryFlagNoRodTimeBonus     = 0x02;
constexpr std::uint32_t LegendaryFlagEbisuTimeBonusOnly = 0x04;
constexpr std::uint32_t LegendaryFlagAddTimeBonus       = 0x08;

// FISHINGSIZETYPE
constexpr std::uint8_t SizeTypeSmall = 0;
constexpr std::uint8_t SizeTypeLarge = 1;

// FISHINGSENSETYPE
constexpr std::uint8_t SenseTypeGood                   = 1;
constexpr std::uint8_t SenseTypeBad                    = 2;
constexpr std::uint8_t SenseTypeTerrible               = 3;
constexpr std::uint8_t SenseTypeNoSkillFeeling         = 4;
constexpr std::uint8_t SenseTypeNoSkillSureFeeling     = 5;
constexpr std::uint8_t SenseTypeNoSkillPositiveFeeling = 6;

// FISHINGFAILTYPE
constexpr std::uint8_t FailTypeLostTooSmall = 7;
constexpr std::uint8_t FailTypeLostLowSkill = 8;

// FISHINGNM flag bits.
constexpr std::uint32_t FishingNMRandomRegenEasy       = 0x01;
constexpr std::uint32_t FishingNMRandomRegenDifficult  = 0x02;
constexpr std::uint32_t FishingNMRandomHealEasy        = 0x04;
constexpr std::uint32_t FishingNMRandomHealDifficult   = 0x08;
constexpr std::uint32_t FishingNMRandomAttackEasy      = 0x10;
constexpr std::uint32_t FishingNMRandomAttackDifficult = 0x20;

// MOONPATTERN_3: clamp(1.0 - (x/7), 0, 1) with integer x/7.
inline auto MoonPattern3(const std::uint8_t x) -> float
{
    const auto v = 1.0f - static_cast<float>(static_cast<int>(x) / 7);
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

// Pure GetHookTime wait (moon/hour/rope injects).
inline auto GetHookTimeWait(const std::uint8_t moonPhase, const std::uint8_t hour, const bool fishersRope) -> std::uint8_t
{
    auto waitTime = 13;
    if (moonPhase == MoonPhaseNew || moonPhase == MoonPhaseFull)
    {
        waitTime -= 4;
    }
    if (hour == 5 || hour == 17)
    {
        waitTime -= 1;
    }
    if (fishersRope)
    {
        waitTime -= 1;
    }
    if (waitTime < 7)
    {
        return 7;
    }
    return static_cast<std::uint8_t>(waitTime);
}

// Pure CalculateHookTime with host injects.
inline auto CalculateHookTime(const std::uint8_t fishTime, const std::uint8_t lgdBonusTime, const std::uint8_t multiplier, const std::uint32_t rodFlags,
                              const std::uint16_t rodID, const std::uint8_t sizeType, const bool legendary, const std::uint32_t legendaryFlags,
                              const bool hasMooching, const bool liveBait, const bool albatrossRing) -> std::uint8_t
{
    auto hookTime = fishTime;

    if ((sizeType == SizeTypeLarge && (rodFlags & RodFlagLargePenalty) != 0) || (sizeType == SizeTypeSmall && (rodFlags & RodFlagSmallPenalty) != 0))
    {
        hookTime = static_cast<std::uint8_t>(hookTime - 10);
    }

    if (legendary && (rodFlags & RodFlagLegendaryBonus) != 0)
    {
        hookTime = static_cast<std::uint8_t>(hookTime + 10);
    }

    if (hasMooching && liveBait)
    {
        hookTime = static_cast<std::uint8_t>(hookTime + 30);
    }

    if (albatrossRing)
    {
        hookTime = static_cast<std::uint8_t>(hookTime + 30);
    }

    if (legendary)
    {
        if ((legendaryFlags & LegendaryFlagNoRodTimeBonus) != 0 ||
            ((legendaryFlags & LegendaryFlagEbisuTimeBonusOnly) != 0 && rodID == RodIDEbisu))
        {
            hookTime = static_cast<std::uint8_t>(hookTime + lgdBonusTime);
        }

        if ((legendaryFlags & LegendaryFlagHalfTime) != 0)
        {
            hookTime = static_cast<std::uint8_t>(hookTime - static_cast<std::uint8_t>(fishTime / 2));
        }

        if ((legendaryFlags & LegendaryFlagAddTimeBonus) != 0)
        {
            hookTime = static_cast<std::uint8_t>(hookTime + (multiplier & 10));
        }
    }

    return hookTime;
}

// Pure CalculateCriticalBite with discrete moon phase inject.
inline auto CalculateCriticalBite(const std::uint8_t fishingSkill, const std::uint8_t fishSkill, const std::uint16_t rodID, const std::uint8_t moonPhase)
    -> std::uint16_t
{
    auto ebisuBonus = 0;
    if (rodID == RodIDEbisu)
    {
        ebisuBonus = 40;
    }

    if (static_cast<int>(fishSkill) - 4 > static_cast<int>(fishingSkill) + ebisuBonus)
    {
        return 0;
    }

    auto fishSkillCheck = static_cast<int>(fishSkill) - 4;
    if (fishSkillCheck < 0)
    {
        fishSkillCheck = 0;
    }

    auto skillDiff = static_cast<int>(fishingSkill) + ebisuBonus - fishSkillCheck;
    if (skillDiff < 0)
    {
        skillDiff = 0;
    }

    auto chance = 5 + skillDiff * 2;

    const auto moonModifier = 2.0f * MoonPattern3(moonPhase);
    chance += static_cast<int>(static_cast<std::uint16_t>(10.0f * (2.0f - moonModifier)));

    if (chance < 0)
    {
        chance = 0;
    }
    if (chance > 70)
    {
        chance = 70;
    }
    return static_cast<std::uint16_t>(chance);
}

// Pure weather modifier (Weather enum as uint16).
inline auto GetWeatherModifier(const std::uint16_t weather) -> float
{
    if (weather == WeatherRain)
    {
        return 1.1f;
    }
    if (weather == WeatherSquall)
    {
        return 1.2f;
    }
    return 1.0f;
}

inline auto IsLiveBait(const std::uint16_t baitID) -> bool
{
    return baitID == BaitIDDrillCalamary || baitID == BaitIDDwarfPugil;
}

struct LsbRet
{
    std::uint8_t failReason = 0;
    std::uint8_t chance     = 0;
};

// Pure fish-sense selection after lose/snap/break chances known.
// roll02/rollBad02 are GetRandomNumber<uint16>(2) results in {0,1}.
inline auto CalculateFishSense(const LsbRet& lose, const LsbRet& snap, const LsbRet& brk, const std::uint8_t roll02, const std::uint8_t rollBad02)
    -> std::uint8_t
{
    auto sense = SenseTypeGood;

    if (lose.chance > 0 && snap.chance == 0 && brk.chance == 0)
    {
        if (lose.failReason == FailTypeLostTooSmall)
        {
            sense = SenseTypeGood;
        }
        else
        {
            if (lose.chance < 20)
            {
                sense = static_cast<std::uint8_t>(SenseTypeNoSkillFeeling + roll02);
            }
            else if (lose.chance < 45)
            {
                sense = SenseTypeNoSkillSureFeeling;
            }
            else
            {
                sense = SenseTypeNoSkillPositiveFeeling;
            }
        }
    }
    else if (snap.chance > 0 || brk.chance > 0)
    {
        if (snap.chance < 30 && brk.chance < 30)
        {
            sense = SenseTypeBad;
        }
        else if (snap.chance < 45 && brk.chance < 45)
        {
            sense = static_cast<std::uint8_t>(SenseTypeBad + rollBad02);
        }
        else
        {
            sense = SenseTypeTerrible;
        }
    }

    return sense;
}

struct BigFishStats
{
    std::uint16_t length = 0;
    std::uint16_t weight = 0;
    bool          epic   = false;
};

// Pure big-fish stats with RNG injects.
inline auto CalculateBigFishStats(const std::uint16_t minLength, const std::uint16_t maxLength, const std::uint16_t lengthRoll1, const std::uint16_t lengthRoll2,
                                  const float weightRandomizer) -> BigFishStats
{
    BigFishStats stats{};
    if (maxLength <= 1)
    {
        return stats;
    }

    stats.length = static_cast<std::uint16_t>((static_cast<int>(lengthRoll1) + static_cast<int>(lengthRoll2)) / 2);
    const auto w = static_cast<std::int16_t>(std::floor(static_cast<float>(stats.length) * weightRandomizer));
    stats.weight = static_cast<std::uint16_t>(w);

    const auto mid = static_cast<std::uint16_t>((static_cast<int>(minLength) + static_cast<int>(maxLength)) / 2);
    if (stats.length > mid && weightRandomizer >= 5.0f)
    {
        stats.epic = true;
    }
    return stats;
}

struct FishMobModifiers
{
    std::uint8_t regenBonus    = 0;
    std::uint8_t attackPenalty = 0;
    std::uint8_t healBonus     = 0;
};

struct MobModifierRolls
{
    std::uint16_t regenEasy       = 0;
    std::uint16_t regenDifficult  = 0;
    std::uint16_t healEasy        = 0;
    std::uint16_t healDifficult   = 0;
    std::uint16_t attackEasy      = 0;
    std::uint16_t attackDifficult = 0;
};

// Pure mob modifiers with nmFlags and RNG injects.
inline auto CalculateMobModifiers(const std::uint32_t nmFlags, const MobModifierRolls& rolls) -> FishMobModifiers
{
    FishMobModifiers m{};

    if ((nmFlags & FishingNMRandomRegenEasy) != 0)
    {
        m.regenBonus = static_cast<std::uint8_t>(m.regenBonus + static_cast<std::uint8_t>(rolls.regenEasy));
    }
    if ((nmFlags & FishingNMRandomRegenDifficult) != 0)
    {
        m.regenBonus = static_cast<std::uint8_t>(m.regenBonus + static_cast<std::uint8_t>(rolls.regenDifficult));
    }
    if ((nmFlags & FishingNMRandomHealEasy) != 0)
    {
        m.healBonus = static_cast<std::uint8_t>(m.healBonus + static_cast<std::uint8_t>(rolls.healEasy));
    }
    if ((nmFlags & FishingNMRandomHealDifficult) != 0)
    {
        m.healBonus = static_cast<std::uint8_t>(m.healBonus + static_cast<std::uint8_t>(rolls.healDifficult));
    }
    if ((nmFlags & FishingNMRandomAttackEasy) != 0)
    {
        m.attackPenalty = static_cast<std::uint8_t>(m.attackPenalty + static_cast<std::uint8_t>(rolls.attackEasy));
    }
    if ((nmFlags & FishingNMRandomAttackDifficult) != 0)
    {
        m.attackPenalty = static_cast<std::uint8_t>(m.attackPenalty + static_cast<std::uint8_t>(rolls.attackDifficult));
    }

    return m;
}

} // namespace fishinghookhelpers
