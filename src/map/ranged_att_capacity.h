#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CBattleEntity::RATT(uint16 bonusAtt) ranged attack assembly with fully
// injected inputs (weakness, PC weapon/ammo gates, skill, STR, mods, food,
// rangedAttackBonuses). Velocity Shot is not reimplemented — inject
// battleutils::GetRangedAttackBonuses as int32.
// Parity: internal/rangedatt (slice 1641).
//
// Reference: src/map/entities/battle_entity.cpp RATT()

namespace rangedatthelpers
{

// SKILLTYPE pins (battle_entity.h) used by RATT.
constexpr std::uint8_t SkillNone             = 0;
constexpr std::uint8_t SkillAutomatonRanged  = 23;
constexpr std::uint8_t SkillArchery          = 25;
constexpr std::uint8_t SkillMarksmanship     = 26;
constexpr std::uint8_t SkillThrowing         = 27;
constexpr std::uint8_t SkillFishing          = 48;

// DamageType::None pin.
constexpr std::uint8_t DamageTypeNone = 0;

// Base RATT before skill / mods.
constexpr std::int32_t BaseRATT = 8;

// Default multipliers.
constexpr double DefaultNonPCSTRMultiplier        = 0.5;
constexpr double DefaultTrustSTRMultiplier        = 0.75;
constexpr double DefaultRangedSTRAttackMultiplier = 1.0;

// Weakness power threshold for RATT = 0.
constexpr std::uint16_t WeaknessRATTZeroPower = 2;

// ShouldReturnZeroRATTWeakness: hasWeakness && power >= 2.
constexpr auto ShouldReturnZeroRATTWeakness(const bool hasWeakness, const std::uint16_t weaknessPower) -> bool
{
    return hasWeakness && weaknessPower >= WeaknessRATTZeroPower;
}

// PC ranged-without-ammo gate.
constexpr auto ShouldReturnZeroPCNoAmmo(const bool        hasRangedWeapon,
                                        const std::uint8_t rangedSkillType,
                                        const bool        hasAmmoWeapon) -> bool
{
    return hasRangedWeapon && rangedSkillType != SkillThrowing && !hasAmmoWeapon;
}

// Non-damaging weapon gate.
constexpr auto ShouldReturnZeroNonDamagingWeapon(const std::uint8_t dmgType, const std::uint8_t skillType) -> bool
{
    return dmgType == DamageTypeNone || skillType == SkillNone;
}

// Fully injected PC weapon/ammo state.
struct PCRangedWeaponParams
{
    bool          hasRangedWeapon{};
    std::uint8_t  rangedSkillType{};
    std::uint8_t  rangedDmgType{};
    std::uint16_t rangedGetSkill{};
    std::uint16_t rangedILvlSkill{};
    bool          hasAmmoWeapon{};
    std::uint8_t  ammoSkillType{};
    std::uint8_t  ammoDmgType{};
    std::uint16_t ammoGetSkill{};
    std::uint16_t ammoILvlSkill{};
};

// Result of ResolvePCRangedWeaponSkillLevel.
struct PCRangedSkillResult
{
    std::uint16_t skillLevel{};
    bool          returnZero{};
};

// ResolvePCRangedWeaponSkillLevel: PC weapon selection / skill path.
constexpr auto ResolvePCRangedWeaponSkillLevel(const PCRangedWeaponParams& p) -> PCRangedSkillResult
{
    if (ShouldReturnZeroPCNoAmmo(p.hasRangedWeapon, p.rangedSkillType, p.hasAmmoWeapon))
    {
        return { 0, true };
    }

    const bool useRanged = p.hasRangedWeapon;
    const bool useAmmo   = !useRanged && p.hasAmmoWeapon;
    if (!useRanged && !useAmmo)
    {
        return { 0, true };
    }

    std::uint8_t  skillType{};
    std::uint8_t  dmgType{};
    std::uint16_t getSkill{};
    std::uint16_t iLvlSkill{};
    if (useRanged)
    {
        skillType = p.rangedSkillType;
        dmgType   = p.rangedDmgType;
        getSkill  = p.rangedGetSkill;
        iLvlSkill = p.rangedILvlSkill;
    }
    else
    {
        skillType = p.ammoSkillType;
        dmgType   = p.ammoDmgType;
        getSkill  = p.ammoGetSkill;
        iLvlSkill = p.ammoILvlSkill;
    }

    if (ShouldReturnZeroNonDamagingWeapon(dmgType, skillType))
    {
        return { 0, true };
    }

    std::uint16_t skillLevel = 0;
    if (skillType != SkillFishing)
    {
        skillLevel = static_cast<std::uint16_t>(getSkill + iLvlSkill);
    }
    return { skillLevel, false };
}

// Max(archery, marksmanship, throwing) for Trust path.
constexpr auto MaxRangedSkill(const std::uint16_t archery,
                              const std::uint16_t marksmanship,
                              const std::uint16_t throwing) -> std::uint16_t
{
    auto m = archery;
    if (marksmanship > m)
    {
        m = marksmanship;
    }
    if (throwing > m)
    {
        m = throwing;
    }
    return m;
}

struct EntitySkillSTRResult
{
    std::uint16_t skillLevel{};
    double        strMultiplier{ DefaultNonPCSTRMultiplier };
};

// Resolve entity skillLevel + strMultiplier (post-PC-gate).
constexpr auto ResolveEntitySkillAndSTRMultiplier(const bool          isPC,
                                                  const bool          isAutomatonPet,
                                                  const bool          isTrust,
                                                  const std::uint16_t pcSkillLevel,
                                                  const double        rangedSTRMult,
                                                  const std::uint16_t automatonRangedSkill,
                                                  const std::uint16_t archery,
                                                  const std::uint16_t marksmanship,
                                                  const std::uint16_t throwing) -> EntitySkillSTRResult
{
    if (isPC)
    {
        return { pcSkillLevel, rangedSTRMult };
    }
    if (isAutomatonPet)
    {
        return { automatonRangedSkill, DefaultNonPCSTRMultiplier };
    }
    if (isTrust)
    {
        return { MaxRangedSkill(archery, marksmanship, throwing), DefaultTrustSTRMultiplier };
    }
    return { 0, DefaultNonPCSTRMultiplier };
}

// std::floor(STR * strMultiplier) → int32.
inline auto FloorSTRContribution(const std::uint16_t str, const double strMultiplier) -> std::int32_t
{
    return static_cast<std::int32_t>(std::floor(static_cast<double>(str) * strMultiplier));
}

// min<int16>((RATT * FOOD_RATTP / 100.f), FOOD_RATT_CAP).
constexpr auto FoodRATTBonus(const std::int32_t ratt, const std::int16_t foodRATTP, const std::int16_t foodRATTCap)
    -> std::int16_t
{
    const auto percentF = static_cast<float>(ratt * static_cast<std::int32_t>(foodRATTP)) / 100.0f;
    const auto p16      = static_cast<std::int16_t>(percentF);
    return p16 < foodRATTCap ? p16 : foodRATTCap;
}

// max<int16>(1, RATT + RATT*RATTP/100.f + food).
constexpr auto FinalizeRATT(const std::int32_t ratt,
                            const std::int32_t rattP,
                            const std::int16_t foodRATTP,
                            const std::int16_t foodRATTCap) -> std::uint16_t
{
    const auto food   = static_cast<std::int32_t>(FoodRATTBonus(ratt, foodRATTP, foodRATTCap));
    const auto totalF = static_cast<float>(ratt) + static_cast<float>(ratt * rattP) / 100.0f + static_cast<float>(food);
    auto       total  = static_cast<std::int16_t>(totalF);
    if (total < 1)
    {
        total = 1;
    }
    return static_cast<std::uint16_t>(total);
}

// Fully injected RATT params (no entity graph).
struct RATTParams
{
    bool          returnZero{};
    std::uint16_t bonusAtt{};
    std::int16_t  rattMod{};
    std::int32_t  rattP{};
    std::uint16_t str{};
    double        strMultiplier{ DefaultNonPCSTRMultiplier };
    std::uint16_t skillLevel{};
    std::int32_t  rangedAttackBonuses{};
    std::int16_t  foodRATTP{};
    std::int16_t  foodRATTCap{};
};

// ResolveRATT: pure CBattleEntity::RATT assembly.
inline auto ResolveRATT(const RATTParams& p) -> std::uint16_t
{
    if (p.returnZero)
    {
        return 0;
    }
    const auto ratt = BaseRATT + static_cast<std::int32_t>(p.skillLevel) + static_cast<std::int32_t>(p.bonusAtt) +
                      static_cast<std::int32_t>(p.rattMod) + p.rangedAttackBonuses +
                      FloorSTRContribution(p.str, p.strMultiplier);
    return FinalizeRATT(ratt, p.rattP, p.foodRATTP, p.foodRATTCap);
}

} // namespace rangedatthelpers
