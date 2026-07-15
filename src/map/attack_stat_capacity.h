#pragma once

#include <algorithm>
#include <cstdint>

// Pure CBattleEntity::ATT(SLOTTYPE) melee attack assembly with fully injected
// inputs (STR, skill, weapon flags, mods, settings multipliers).
// Parity: internal/attackstat (slice 1639).
//
// Reference: src/map/entities/battle_entity.cpp ATT()

namespace attackstathelpers
{

// SLOTTYPE pins (weapon slots used by ATT).
constexpr std::uint8_t SlotMain   = 0x00;
constexpr std::uint8_t SlotSub    = 0x01;
constexpr std::uint8_t SlotRanged = 0x02;
constexpr std::uint8_t SlotAmmo   = 0x03;

// Base ATT before mods.
constexpr std::int32_t BaseATT = 8;

// Non-PC / unmatched STR→ATT multiplier.
constexpr float DefaultNonPCSTRMultiplier = 0.5f;

// settings/default/main.lua defaults.
constexpr float DefaultTwoHandedSTRAttackMultiplier   = 1.0f;
constexpr float DefaultHandToHandSTRAttackMultiplier  = 1.0f;
constexpr float DefaultOneHandMainSTRAttackMultiplier = 0.75f;
constexpr float DefaultOneHandOffSTRAttackMultiplier  = 0.5f;
constexpr float DefaultRangedSTRAttackMultiplier      = 1.0f;

// ResolveSTRAttackMultiplier mirrors the objtype/weapon/slot branch for strMultiplier.
constexpr auto ResolveSTRAttackMultiplier(const bool        isPC,
                                          const bool        hasWeapon,
                                          const bool        isTwoHand,
                                          const bool        isH2H,
                                          const std::uint8_t slot,
                                          const float       twoHandedMult,
                                          const float       h2hMult,
                                          const float       oneHandMainMult,
                                          const float       oneHandOffMult,
                                          const float       rangedMult) -> float
{
    if (!isPC)
    {
        return DefaultNonPCSTRMultiplier;
    }
    if (hasWeapon && isTwoHand)
    {
        return twoHandedMult;
    }
    if (hasWeapon && isH2H)
    {
        return h2hMult;
    }
    if (slot == SlotMain)
    {
        return oneHandMainMult;
    }
    if (slot == SlotSub)
    {
        return oneHandOffMult;
    }
    if (slot == SlotRanged || slot == SlotAmmo)
    {
        return rangedMult;
    }
    return DefaultNonPCSTRMultiplier;
}

// ApplySmiteToATTP: ATTP += static_cast<int32>(smite / 256.0f * 100) when 2H/H2H.
constexpr auto ApplySmiteToATTP(const std::int32_t attP, const std::int16_t smiteMod, const bool isTwoHandOrH2H)
    -> std::int32_t
{
    if (!isTwoHandOrH2H)
    {
        return attP;
    }
    const auto bonus = static_cast<std::int32_t>(static_cast<float>(smiteMod) / 256.0f * 100.0f);
    return attP + bonus;
}

constexpr auto SmiteATTPBonus(const std::int16_t smiteMod) -> std::int32_t
{
    return static_cast<std::int32_t>(static_cast<float>(smiteMod) / 256.0f * 100.0f);
}

// TruncSTRContribution: ATT += STR * strMultiplier (float → int32, toward 0).
constexpr auto TruncSTRContribution(const std::uint16_t str, const float strMultiplier) -> std::int32_t
{
    return static_cast<std::int32_t>(static_cast<float>(str) * strMultiplier);
}

// FoodATTBonus: std::min<int16>((ATT * FOOD_ATTP / 100), FOOD_ATT_CAP).
constexpr auto FoodATTBonus(const std::int32_t att, const std::int16_t foodATTP, const std::int16_t foodATTCap)
    -> std::int16_t
{
    const auto percent = static_cast<std::int16_t>(att * static_cast<std::int32_t>(foodATTP) / 100);
    return percent < foodATTCap ? percent : foodATTCap;
}

// FinalizeATT: max(1, ATT + ATT*ATTP/100 + food).
constexpr auto FinalizeATT(const std::int32_t att,
                           const std::int32_t attP,
                           const std::int16_t foodATTP,
                           const std::int16_t foodATTCap) -> std::uint16_t
{
    const auto food  = static_cast<std::int32_t>(FoodATTBonus(att, foodATTP, foodATTCap));
    auto       total = att + (att * attP / 100) + food;
    if (total < 1)
    {
        total = 1;
    }
    return static_cast<std::uint16_t>(total);
}

// Fully injected ATT params (no entity graph).
struct ATTParams
{
    std::int16_t  attMod{};
    std::int32_t  attP{};
    std::uint16_t str{};
    float         strMultiplier{ DefaultNonPCSTRMultiplier };

    bool         hasEndark{};
    std::int16_t enspellDMG{};

    bool isPC{};
    bool isPet{};
    bool isAutomaton{};
    bool hasWeapon{};

    std::uint16_t weaponSkill{};
    std::uint16_t weaponILvlSkill{};
    bool          isTwoHandOrH2H{};
    std::int16_t  smiteMod{};

    std::uint16_t automatonMeleeSkill{};

    // Pre-resolved ecosystem inject (do not depend on ecosystem package here).
    bool         monsterCorrelationAdvantage{};
    std::int16_t enhancesMonsterCorrelation{};

    std::int16_t foodATTP{};
    std::int16_t foodATTCap{};
};

// ResolveATT: pure CBattleEntity::ATT assembly.
constexpr auto ResolveATT(const ATTParams& p) -> std::uint16_t
{
    auto att  = BaseATT + static_cast<std::int32_t>(p.attMod);
    auto attP = p.attP;

    att += TruncSTRContribution(p.str, p.strMultiplier);

    if (p.hasEndark)
    {
        att += static_cast<std::int32_t>(p.enspellDMG);
    }

    if (p.isPC)
    {
        if (p.hasWeapon)
        {
            att += static_cast<std::int32_t>(p.weaponSkill) + static_cast<std::int32_t>(p.weaponILvlSkill);
            attP = ApplySmiteToATTP(attP, p.smiteMod, p.isTwoHandOrH2H);
        }
    }
    else if (p.isPet && p.isAutomaton)
    {
        att += static_cast<std::int32_t>(p.automatonMeleeSkill);
    }
    else if (p.isPet)
    {
        if (p.monsterCorrelationAdvantage)
        {
            attP += static_cast<std::int32_t>(p.enhancesMonsterCorrelation);
        }
    }

    return FinalizeATT(att, attP, p.foodATTP, p.foodATTCap);
}

} // namespace attackstathelpers
