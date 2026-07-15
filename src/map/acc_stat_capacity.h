#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CBattleEntity::ACC / GetAccFromSkill melee accuracy assembly with fully
// injected inputs (DEX, skill, weapon flags, mods, merits, tandem, food,
// settings DEX multipliers).
// Parity: internal/accstat (slice 1640).
//
// Reference: src/map/entities/battle_entity.cpp
//   GetAccFromSkill (~1320–1338)
//   CBattleEntity::ACC (~1455–1611)

namespace accstathelpers
{

// settings/default/main.lua defaults.
constexpr float DefaultTwoHandedDEXAccuracyMultiplier   = 0.75f;
constexpr float DefaultHandToHandDEXAccuracyMultiplier  = 0.75f;
constexpr float DefaultOneHandMainDEXAccuracyMultiplier = 0.75f;
constexpr float DefaultOneHandOffDEXAccuracyMultiplier  = 0.75f;

// Hard-coded automaton / unset PC dex multiplier.
constexpr float DefaultAutomatonDEXMultiplier = 0.5f;
constexpr float DefaultUnsetDEXMultiplier     = 0.5f;

// GetAccFromSkill: skill→accuracy curve.
inline auto GetAccFromSkill(const std::uint32_t skill) -> std::uint32_t
{
    if (skill > 600)
    {
        return static_cast<std::uint32_t>(std::floor(static_cast<float>(skill - 600.f) * 0.9f)) + 540;
    }
    if (skill > 400)
    {
        return static_cast<std::uint32_t>(std::floor(static_cast<float>(skill - 400.f) * 0.8f)) + 380;
    }
    if (skill > 200)
    {
        return static_cast<std::uint32_t>(std::floor(static_cast<float>(skill - 200.f) * 0.9f)) + 200;
    }
    return skill;
}

// ResolveAttackDEXMultiplier: PC attackNumber / weapon-flag branch before 2H override.
constexpr auto ResolveAttackDEXMultiplier(const std::uint8_t attackNumber,
                                          const bool         hasMainWeapon,
                                          const bool         mainIsH2H,
                                          const bool         mainSkillIsNone,
                                          const bool         h2hSkillGtZero,
                                          const bool         hasSubWeapon,
                                          const bool         subSkillIsNone,
                                          const bool         mainSkillIsNoneOrH2H,
                                          const float        oneHandMain,
                                          const float        oneHandOff,
                                          const float        h2h) -> float
{
    float dexMult = DefaultUnsetDEXMultiplier;

    if (attackNumber == 0)
    {
        dexMult = oneHandMain;
        if (hasMainWeapon && ((mainSkillIsNone && h2hSkillGtZero) || mainIsH2H))
        {
            dexMult = h2h;
        }
    }
    else if (attackNumber == 1)
    {
        if (hasSubWeapon)
        {
            dexMult = oneHandOff;
            if (subSkillIsNone && h2hSkillGtZero && hasMainWeapon && mainSkillIsNoneOrH2H)
            {
                dexMult = h2h;
            }
        }
        else if (hasMainWeapon && mainIsH2H)
        {
            dexMult = h2h;
        }
    }
    else if (attackNumber == 2)
    {
        dexMult = h2h;
    }

    return dexMult;
}

// TruncDEXContribution: ACC += floor(DEX * mult).
inline auto TruncDEXContribution(const std::uint16_t dex, const float dexMultiplier) -> std::int32_t
{
    return static_cast<std::int32_t>(std::floor(static_cast<float>(dex) * dexMultiplier));
}

// FoodPCBonus: min<int16>((ACC * FOOD_ACCP / 100.f), FOOD_ACC_CAP).
inline auto FoodPCBonus(const std::int32_t acc, const std::int16_t foodACCP, const std::int16_t foodACCCap)
    -> std::int16_t
{
    const auto percent = static_cast<std::int16_t>(static_cast<float>(acc * static_cast<std::int32_t>(foodACCP)) / 100.f);
    return percent < foodACCCap ? percent : foodACCCap;
}

// FinalizePCACC: max(0, ACC + food).
inline auto FinalizePCACC(const std::int32_t acc, const std::int16_t foodACCP, const std::int16_t foodACCCap)
    -> std::uint16_t
{
    auto total = acc + static_cast<std::int32_t>(FoodPCBonus(acc, foodACCP, foodACCCap));
    if (total < 0)
    {
        total = 0;
    }
    return static_cast<std::uint16_t>(total);
}

// FoodNonPCBonus: min<int16>(((100 + FOOD_ACCP * ACC) / 100), FOOD_ACC_CAP).
inline auto FoodNonPCBonus(const std::int32_t acc, const std::int16_t foodACCP, const std::int16_t foodACCCap)
    -> std::int16_t
{
    const auto percent = static_cast<std::int16_t>((100 + static_cast<std::int32_t>(foodACCP) * acc) / 100);
    return percent < foodACCCap ? percent : foodACCCap;
}

// FinalizeNonPCACC: max(1, ACC + food).
inline auto FinalizeNonPCACC(const std::int32_t acc, const std::int16_t foodACCP, const std::int16_t foodACCCap)
    -> std::uint16_t
{
    auto total = acc + static_cast<std::int32_t>(FoodNonPCBonus(acc, foodACCP, foodACCCap));
    if (total < 1)
    {
        total = 1;
    }
    return static_cast<std::uint16_t>(total);
}

// Fully injected ACC params (no entity graph).
struct ACCParams
{
    bool isPC{};
    bool isPet{};
    bool isAutomaton{};

    std::uint32_t skillLevel{};

    std::uint16_t dex{};
    float         dexMultiplier{ DefaultUnsetDEXMultiplier };
    std::int16_t  accMod{};
    std::uint16_t offsetAccuracy{};

    bool          mainIsTwoHanded{};
    float         twoHandedDEXMultiplier{ DefaultTwoHandedDEXAccuracyMultiplier };
    std::int16_t  twoHandACC{};

    bool         hasEnlight{};
    std::int16_t enspellDMG{};
    std::int16_t tandemStrikePower{};
    std::int16_t meritAccuracy{};

    bool         monsterCorrelationAdvantage{};
    std::int16_t enhancesMonsterCorrelation{};

    std::int16_t foodACCP{};
    std::int16_t foodACCCap{};
};

// ResolveACC: pure CBattleEntity::ACC assembly.
inline auto ResolveACC(const ACCParams& p) -> std::uint16_t
{
    if (p.isPC)
    {
        auto  acc     = static_cast<std::int32_t>(GetAccFromSkill(p.skillLevel));
        float dexMult = p.dexMultiplier;

        if (p.mainIsTwoHanded)
        {
            dexMult = p.twoHandedDEXMultiplier;
            acc += TruncDEXContribution(p.dex, dexMult);
            acc += static_cast<std::int32_t>(p.twoHandACC);
        }
        else
        {
            acc += TruncDEXContribution(p.dex, dexMult);
        }

        acc = acc + static_cast<std::int32_t>(p.accMod) + static_cast<std::int32_t>(p.offsetAccuracy);

        if (p.hasEnlight)
        {
            acc += static_cast<std::int32_t>(p.enspellDMG);
        }
        acc += static_cast<std::int32_t>(p.tandemStrikePower);
        acc += static_cast<std::int32_t>(p.meritAccuracy);

        return FinalizePCACC(acc, p.foodACCP, p.foodACCCap);
    }

    if (p.isPet && p.isAutomaton)
    {
        auto acc = static_cast<std::int32_t>(GetAccFromSkill(p.skillLevel));
        acc += TruncDEXContribution(p.dex, DefaultAutomatonDEXMultiplier);
        acc += static_cast<std::int32_t>(p.accMod) + static_cast<std::int32_t>(p.offsetAccuracy);

        if (p.hasEnlight)
        {
            acc += static_cast<std::int32_t>(p.enspellDMG);
        }
        acc += static_cast<std::int32_t>(p.tandemStrikePower);

        return FinalizeNonPCACC(acc, p.foodACCP, p.foodACCCap);
    }

    auto acc = static_cast<std::int32_t>(p.accMod) + static_cast<std::int32_t>(p.offsetAccuracy);

    if (p.hasEnlight)
    {
        acc += static_cast<std::int32_t>(p.enspellDMG);
    }
    acc += static_cast<std::int32_t>(p.tandemStrikePower);

    if (p.isPet && p.monsterCorrelationAdvantage)
    {
        acc += static_cast<std::int32_t>(p.enhancesMonsterCorrelation);
    }

    acc += static_cast<std::int32_t>(p.dex / 2);

    return FinalizeNonPCACC(acc, p.foodACCP, p.foodACCCap);
}

} // namespace accstathelpers
