#pragma once

#include "automaton_weapon_damage_capacity.h"
#include "pet_weapon_damage_capacity.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CBattleEntity::GetRangedWeaponDmg formulas.
// Parity: internal/rangedweapondmg (slice 1650).
//
// Reference: src/map/entities/battle_entity.cpp GetRangedWeaponDmg (~808–914)
//
// Host retains weapon-slot lookup, getMod / getMobMod / GetSkill, and
// production wire of CBattleEntity::GetRangedWeaponDmg.

namespace rangedweapondmghelpers
{

// Entity-kind pins for ResolveRangedWeaponDmg.
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
//          + rangedDmgRating + rangedDamageOffset
//   clamp to [1, 65535]
inline auto ResolveMobRangedWeaponDmg(const std::int32_t weaponDamage,
                                      const std::int32_t baseDamageModifier,
                                      const std::int32_t dmgMult,
                                      const std::int32_t baseDamageMultiplier,
                                      const std::int32_t rangedDmgRating,
                                      const std::int32_t rangedDamageOffset) -> std::uint16_t
{
    float mult = static_cast<float>(dmgMult) / 100.0f;
    if (baseDamageMultiplier != 0)
    {
        mult = static_cast<float>(baseDamageMultiplier) / 100.0f;
    }
    auto damage = static_cast<std::int32_t>(std::floor((weaponDamage + baseDamageModifier) * mult));
    damage += rangedDmgRating + rangedDamageOffset;
    damage = std::clamp(damage, 1, 65535);
    return static_cast<std::uint16_t>(damage);
}

// TYPE_PET AUTOMATON: pure skill formula + RANGED_DMG_RATING (no clamp; cast to uint16).
inline auto ResolveAutomatonRangedWeaponDmg(const std::uint16_t skill, const std::int32_t rangedDmgRating) -> std::uint16_t
{
    return static_cast<std::uint16_t>(automatonweapondamagehelpers::WeaponDamageWithRating(skill, rangedDmgRating));
}

// TYPE_PET WYVERN: floor(level/2)+3 + RANGED_DMG_RATING.
inline auto ResolveWyvernRangedWeaponDmg(const std::uint8_t level, const std::int32_t rangedDmgRating) -> std::uint16_t
{
    return static_cast<std::uint16_t>(petweapondamagehelpers::WyvernWeaponDamageWithRating(level, rangedDmgRating));
}

// TYPE_PET jug / other: GetJugWeaponDamage(level) + RANGED_DMG_RATING.
inline auto ResolveJugRangedWeaponDmg(const std::uint8_t level, const std::int32_t rangedDmgRating) -> std::uint16_t
{
    return static_cast<std::uint16_t>(petweapondamagehelpers::JugWeaponDamageWithRating(static_cast<float>(level), rangedDmgRating));
}

// TYPE_PET AVATAR: ranged weapon dmg + RANGED_DMG_RATING, clamp [1, 65535].
inline auto ResolveAvatarRangedWeaponDmg(const std::int32_t weaponDamage, const std::int32_t rangedDmgRating) -> std::uint16_t
{
    auto damage = weaponDamage + rangedDmgRating;
    damage      = std::clamp(damage, 1, 65535);
    return static_cast<std::uint16_t>(damage);
}

namespace detail
{
// Optional PC underlevel scale for one ranged/ammo slot.
inline auto SlotBase(const std::uint16_t baseDmg, const std::uint8_t reqLvl, const std::uint8_t mLevel, const bool isPC)
    -> std::uint16_t
{
    if (isPC && reqLvl > mLevel)
    {
        auto dmg = static_cast<std::uint32_t>(baseDmg);
        dmg *= static_cast<std::uint32_t>(mLevel) * 3u;
        dmg /= 4u;
        dmg /= static_cast<std::uint32_t>(reqLvl);
        return static_cast<std::uint16_t>(dmg);
    }
    return baseDmg;
}
} // namespace detail

// PC / other fall-through: ranged slot + ammo slot + RANGED_DMG_RATING.
// Empty slots do not force zero — entity rating is always applied.
// When isPC && reqLvl > mLevel for a slot:
//   scaled = slotDmg * mLevel * 3 / 4 / reqLvl  (uint32 intermediates)
// return sum of slot bases + item ratings + rangedDmgRating (int32 sum → uint16)
inline auto ResolvePCRangedWeaponDmg(const bool          hasRanged,
                                     const std::uint16_t rangedDmg,
                                     const std::uint8_t  rangedReqLvl,
                                     const std::int16_t  rangedItemDmgRating,
                                     const bool          hasAmmo,
                                     const std::uint16_t ammoDmg,
                                     const std::uint8_t  ammoReqLvl,
                                     const std::int16_t  ammoItemDmgRating,
                                     const std::uint8_t  mLevel,
                                     const bool          isPC,
                                     const std::int32_t  rangedDmgRating) -> std::uint16_t
{
    std::int32_t sum = 0;
    if (hasRanged)
    {
        sum += static_cast<std::int32_t>(detail::SlotBase(rangedDmg, rangedReqLvl, mLevel, isPC)) +
               static_cast<std::int32_t>(rangedItemDmgRating);
    }
    if (hasAmmo)
    {
        sum += static_cast<std::int32_t>(detail::SlotBase(ammoDmg, ammoReqLvl, mLevel, isPC)) +
               static_cast<std::int32_t>(ammoItemDmgRating);
    }
    sum += rangedDmgRating;
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
    std::int32_t  rangedDamageOffset{ 0 };
    std::int32_t  rangedDmgRating{ 0 };
    std::uint16_t automatonRangedSkill{ 0 };
    std::uint8_t  mLevel{ 0 };
    bool          hasRanged{ false };
    std::uint16_t rangedWeaponDmg{ 0 };
    std::uint8_t  rangedReqLvl{ 0 };
    std::int16_t  rangedItemDmgRating{ 0 };
    bool          hasAmmo{ false };
    std::uint16_t ammoDmg{ 0 };
    std::uint8_t  ammoReqLvl{ 0 };
    std::int16_t  ammoItemDmgRating{ 0 };
};

// Dispatcher mirroring GetRangedWeaponDmg entity-kind branches.
inline auto ResolveRangedWeaponDmg(const Params& p) -> std::uint16_t
{
    switch (p.kind)
    {
        case EntityKind::Mob:
            return ResolveMobRangedWeaponDmg(p.weaponDamage, p.baseDamageModifier, p.dmgMult, p.baseDamageMultiplier, p.rangedDmgRating, p.rangedDamageOffset);
        case EntityKind::PetAutomaton:
            return ResolveAutomatonRangedWeaponDmg(p.automatonRangedSkill, p.rangedDmgRating);
        case EntityKind::PetWyvern:
            return ResolveWyvernRangedWeaponDmg(p.mLevel, p.rangedDmgRating);
        case EntityKind::PetAvatar:
            return ResolveAvatarRangedWeaponDmg(static_cast<std::int32_t>(p.rangedWeaponDmg), p.rangedDmgRating);
        case EntityKind::PetJug:
            return ResolveJugRangedWeaponDmg(p.mLevel, p.rangedDmgRating);
        case EntityKind::PC:
            return ResolvePCRangedWeaponDmg(p.hasRanged, p.rangedWeaponDmg, p.rangedReqLvl, p.rangedItemDmgRating, p.hasAmmo, p.ammoDmg, p.ammoReqLvl, p.ammoItemDmgRating, p.mLevel, true, p.rangedDmgRating);
        default: // Other
            return ResolvePCRangedWeaponDmg(p.hasRanged, p.rangedWeaponDmg, p.rangedReqLvl, p.rangedItemDmgRating, p.hasAmmo, p.ammoDmg, p.ammoReqLvl, p.ammoItemDmgRating, p.mLevel, false, p.rangedDmgRating);
    }
}

} // namespace rangedweapondmghelpers
