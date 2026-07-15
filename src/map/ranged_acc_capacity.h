#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CBattleEntity::RACC(uint16 bonusAcc) assembly with fully injected
// inputs (AGI, skill, weapon flags, mods, food, tandem/correlation injects).
// Local GetAccFromSkill (ACC slice may host an independent copy).
// Parity: internal/rangedacc (slice 1642).
//
// Reference: src/map/entities/battle_entity.cpp RACC() / GetAccFromSkill()

namespace rangedacchelpers
{

// settings/default/main.lua default.
constexpr float DefaultRangedAGIAccuracyMultiplier = 0.75f;

// Hard-coded non-PC AGI→RACC multipliers.
constexpr float AutomatonAGIAccuracyMultiplier = 0.5f;
constexpr float TrustAGIAccuracyMultiplier     = 0.75f;

// GetAccFromSkill: soft skill→accuracy curve.
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

// Weakness power >= 2 → early RACC 1 (RATT returns 0).
constexpr auto WeaknessBlocksRACC(const bool weaknessActive, const std::uint16_t weaknessPower) -> bool
{
    return weaknessActive && weaknessPower >= 2;
}

// Non-throwing ranged without ammo → early 0.
constexpr auto PCRACCMissingAmmo(const bool hasRangedWeapon, const bool skillIsThrowing, const bool hasAmmoWeapon)
    -> bool
{
    return hasRangedWeapon && !skillIsThrowing && !hasAmmoWeapon;
}

// Missing / non-damaging / SKILL_NONE weapon → early 0.
constexpr auto PCRACCWeaponReject(const bool hasWeapon, const bool dmgTypeNone, const bool skillIsNone) -> bool
{
    if (!hasWeapon)
    {
        return true;
    }
    return dmgTypeNone || skillIsNone;
}

// std::floor(AGI * mult) → int32.
inline auto FloorAGIContribution(const std::uint16_t agi, const float mult) -> std::int32_t
{
    return static_cast<std::int32_t>(std::floor(static_cast<float>(agi) * mult));
}

// Food quirk: min<int16>(((100 + FOOD_RACCP * RACC) / 100), FOOD_RACC_CAP)
// — not RACC * FOOD / 100.
constexpr auto FoodRACCBonus(const std::int32_t racc, const std::int16_t foodRACCP, const std::int16_t foodRACCCap)
    -> std::int16_t
{
    const auto percent = static_cast<std::int16_t>((100 + static_cast<std::int32_t>(foodRACCP) * racc) / 100);
    return percent < foodRACCCap ? percent : foodRACCCap;
}

// max(1, RACC + food).
constexpr auto FinalizeRACC(const std::int32_t racc, const std::int16_t foodRACCP, const std::int16_t foodRACCCap)
    -> std::uint16_t
{
    auto total = racc + static_cast<std::int32_t>(FoodRACCBonus(racc, foodRACCP, foodRACCCap));
    if (total < 1)
    {
        total = 1;
    }
    return static_cast<std::uint16_t>(total);
}

// Fully injected RACC params (no entity graph).
struct RACCParams
{
    bool          weaknessActive{};
    std::uint16_t weaknessPower{};

    bool isPC{};
    bool isPet{};
    bool isAutomaton{};
    bool isTrust{};

    bool hasRangedWeapon{};
    bool hasAmmoWeapon{};
    bool skillIsThrowing{};
    bool hasWeapon{};
    bool dmgTypeNone{};
    bool skillIsNone{};
    bool skillIsFishing{};

    std::uint16_t weaponSkill{};
    std::uint16_t weaponILvlSkill{};

    std::uint16_t bonusAcc{};
    std::uint16_t agi{};
    float         rangedAGIAccMultiplier{ DefaultRangedAGIAccuracyMultiplier };
    std::int16_t  raccMod{};
    std::int16_t  accMod{}; // automaton path
    std::int32_t  rangedAccuracyBonuses{};

    std::uint16_t automatonRangedSkill{};

    std::uint16_t archerySkill{};
    std::uint16_t marksmanshipSkill{};
    std::uint16_t throwingSkill{};

    bool         tandemActive{};
    bool         masterIsPC{};
    std::int16_t tandemStrikePower{};
    bool         monsterCorrelationAdvantage{};
    std::int16_t enhancesMonsterCorrelation{};

    std::int16_t foodRACCP{};
    std::int16_t foodRACCCap{};
};

// ResolveRACC: pure CBattleEntity::RACC assembly.
inline auto ResolveRACC(const RACCParams& p) -> std::uint16_t
{
    if (WeaknessBlocksRACC(p.weaknessActive, p.weaknessPower))
    {
        return 1;
    }

    std::int32_t racc = 0;

    if (p.isPC)
    {
        if (PCRACCMissingAmmo(p.hasRangedWeapon, p.skillIsThrowing, p.hasAmmoWeapon))
        {
            return 0;
        }
        if (PCRACCWeaponReject(p.hasWeapon, p.dmgTypeNone, p.skillIsNone))
        {
            return 0;
        }

        std::uint32_t skillLevel = 0;
        if (!p.skillIsFishing)
        {
            skillLevel = static_cast<std::uint32_t>(p.weaponSkill) + static_cast<std::uint32_t>(p.weaponILvlSkill);
        }
        racc = static_cast<std::int32_t>(GetAccFromSkill(skillLevel));
        racc += static_cast<std::int32_t>(p.raccMod);
        racc += static_cast<std::int32_t>(p.bonusAcc);
        racc += p.rangedAccuracyBonuses;
        racc += FloorAGIContribution(p.agi, p.rangedAGIAccMultiplier);
    }
    else if (p.isPet && p.isAutomaton)
    {
        racc = static_cast<std::int32_t>(GetAccFromSkill(static_cast<std::uint32_t>(p.automatonRangedSkill)));
        racc += FloorAGIContribution(p.agi, AutomatonAGIAccuracyMultiplier);
        racc += static_cast<std::int32_t>(p.accMod) + static_cast<std::int32_t>(p.bonusAcc);
    }
    else if (p.isTrust)
    {
        const auto maxSkill = std::max({ p.archerySkill, p.marksmanshipSkill, p.throwingSkill });
        racc                = static_cast<std::int32_t>(GetAccFromSkill(static_cast<std::uint32_t>(maxSkill)));
        racc += FloorAGIContribution(p.agi, TrustAGIAccuracyMultiplier);
        racc += static_cast<std::int32_t>(p.raccMod) + static_cast<std::int32_t>(p.bonusAcc);
    }
    else
    {
        // pets, mobs
        racc = static_cast<std::int32_t>(p.raccMod) + static_cast<std::int32_t>(p.bonusAcc);
        if (p.tandemActive && p.masterIsPC)
        {
            racc += static_cast<std::int32_t>(p.tandemStrikePower);
        }
        if (p.isPet && p.monsterCorrelationAdvantage)
        {
            racc += static_cast<std::int32_t>(p.enhancesMonsterCorrelation);
        }
        racc += static_cast<std::int32_t>(p.agi / 2);
    }

    return FinalizeRACC(racc, p.foodRACCP, p.foodRACCCap);
}

} // namespace rangedacchelpers
