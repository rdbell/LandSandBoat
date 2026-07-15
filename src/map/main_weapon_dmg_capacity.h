#pragma once

#include "automaton_weapon_damage_capacity.h"
#include "pet_weapon_damage_capacity.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CBattleEntity::GetMainWeaponDmg formulas.
// Parity: internal/mainweapondmg (slice 1647).
//
// Reference: src/map/entities/battle_entity.cpp GetMainWeaponDmg (~656–747)
//
// Host retains weapon-slot lookup, getMod / getMobMod / GetSkill, and
// production wire of CBattleEntity::GetMainWeaponDmg.

namespace mainweapondmghelpers
{

// Entity-kind pins for ResolveMainWeaponDmg.
enum class EntityKind : std::uint8_t
{
    PC = 0,
    Mob,
    PetAutomaton,
    PetWyvern,
    PetAvatar,
    PetJug,
    Other,
};

// TYPE_MOB branch:
//   mult = dmgMult/100, or baseDamageMultiplier/100 when baseDamageMultiplier != 0
//   damage = floor((weaponDamage + baseDamageModifier) * mult)
//          + mainDmgRating + damageOffset
//   clamp to [1, 65535]
inline auto ResolveMobMainWeaponDmg(const std::int32_t weaponDamage,
                                    const std::int32_t baseDamageModifier,
                                    const std::int32_t dmgMult,
                                    const std::int32_t baseDamageMultiplier,
                                    const std::int32_t mainDmgRating,
                                    const std::int32_t damageOffset) -> std::uint16_t
{
    float mult = static_cast<float>(dmgMult) / 100.0f;
    if (baseDamageMultiplier != 0)
    {
        mult = static_cast<float>(baseDamageMultiplier) / 100.0f;
    }
    auto damage = static_cast<std::int32_t>(std::floor((weaponDamage + baseDamageModifier) * mult));
    damage += mainDmgRating + damageOffset;
    damage = std::clamp(damage, 1, 65535);
    return static_cast<std::uint16_t>(damage);
}

// TYPE_PET AUTOMATON: pure skill formula + MAIN_DMG_RATING (no clamp; cast to uint16).
inline auto ResolveAutomatonMainWeaponDmg(const std::uint16_t skill, const std::int32_t mainDmgRating) -> std::uint16_t
{
    return static_cast<std::uint16_t>(automatonweapondamagehelpers::WeaponDamageWithRating(skill, mainDmgRating));
}

// TYPE_PET WYVERN: floor(level/2)+3 + MAIN_DMG_RATING.
inline auto ResolveWyvernMainWeaponDmg(const std::uint8_t level, const std::int32_t mainDmgRating) -> std::uint16_t
{
    return static_cast<std::uint16_t>(petweapondamagehelpers::WyvernWeaponDamageWithRating(level, mainDmgRating));
}

// TYPE_PET jug / other: GetJugWeaponDamage(level) + MAIN_DMG_RATING.
inline auto ResolveJugMainWeaponDmg(const std::uint8_t level, const std::int32_t mainDmgRating) -> std::uint16_t
{
    return static_cast<std::uint16_t>(petweapondamagehelpers::JugWeaponDamageWithRating(static_cast<float>(level), mainDmgRating));
}

// TYPE_PET AVATAR: weapon dmg + MAIN_DMG_RATING, clamp [1, 65535].
inline auto ResolveAvatarMainWeaponDmg(const std::int32_t weaponDamage, const std::int32_t mainDmgRating) -> std::uint16_t
{
    auto damage = weaponDamage + mainDmgRating;
    damage      = std::clamp(damage, 1, 65535);
    return static_cast<std::uint16_t>(damage);
}

// PC / other fall-through (hasWeapon false → 0).
// When isPC && reqLvl > mLevel:
//   scaled = weaponDmg * mLevel * 3 / 4 / reqLvl  (uint32 intermediates)
// return scaled + itemDmgRating + mainDmgRating  (int32 sum → uint16)
inline auto ResolvePCMainWeaponDmg(const bool          hasWeapon,
                                   const std::uint16_t weaponDmg,
                                   const std::uint8_t  reqLvl,
                                   const std::uint8_t  mLevel,
                                   const bool          isPC,
                                   const std::int16_t  itemDmgRating,
                                   const std::int32_t  mainDmgRating) -> std::uint16_t
{
    if (!hasWeapon)
    {
        return 0;
    }
    std::uint16_t base = weaponDmg;
    if (isPC && reqLvl > mLevel)
    {
        auto dmg = static_cast<std::uint32_t>(weaponDmg);
        dmg *= static_cast<std::uint32_t>(mLevel) * 3u;
        dmg /= 4u;
        dmg /= static_cast<std::uint32_t>(reqLvl);
        base = static_cast<std::uint16_t>(dmg);
    }
    const auto sum = static_cast<std::int32_t>(base) + static_cast<std::int32_t>(itemDmgRating) + mainDmgRating;
    return static_cast<std::uint16_t>(sum);
}

// Fully-injected parameter pack for the dispatcher.
struct Params
{
    EntityKind    kind{ EntityKind::Other };
    std::int32_t  weaponDamage{ 0 };
    std::int32_t  baseDamageModifier{ 0 };
    std::int32_t  dmgMult{ 100 };
    std::int32_t  baseDamageMultiplier{ 0 };
    std::int32_t  damageOffset{ 0 };
    std::int32_t  mainDmgRating{ 0 };
    std::uint16_t automatonMeleeSkill{ 0 };
    std::uint8_t  mLevel{ 0 };
    bool          hasWeapon{ false };
    std::uint16_t weaponDmg{ 0 };
    std::uint8_t  reqLvl{ 0 };
    std::int16_t  itemDmgRating{ 0 };
};

// Dispatcher mirroring GetMainWeaponDmg entity-kind branches.
inline auto ResolveMainWeaponDmg(const Params& p) -> std::uint16_t
{
    switch (p.kind)
    {
        case EntityKind::Mob:
            return ResolveMobMainWeaponDmg(p.weaponDamage, p.baseDamageModifier, p.dmgMult, p.baseDamageMultiplier, p.mainDmgRating, p.damageOffset);
        case EntityKind::PetAutomaton:
            return ResolveAutomatonMainWeaponDmg(p.automatonMeleeSkill, p.mainDmgRating);
        case EntityKind::PetWyvern:
            return ResolveWyvernMainWeaponDmg(p.mLevel, p.mainDmgRating);
        case EntityKind::PetAvatar:
            return ResolveAvatarMainWeaponDmg(static_cast<std::int32_t>(p.weaponDmg), p.mainDmgRating);
        case EntityKind::PetJug:
            return ResolveJugMainWeaponDmg(p.mLevel, p.mainDmgRating);
        case EntityKind::PC:
            return ResolvePCMainWeaponDmg(p.hasWeapon, p.weaponDmg, p.reqLvl, p.mLevel, true, p.itemDmgRating, p.mainDmgRating);
        default: // Other
            return ResolvePCMainWeaponDmg(p.hasWeapon, p.weaponDmg, p.reqLvl, p.mLevel, false, p.itemDmgRating, p.mainDmgRating);
    }
}

} // namespace mainweapondmghelpers
