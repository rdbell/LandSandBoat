#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CBattleEntity::GetSubWeaponDmg formulas.
// Parity: internal/subweapondmg (slice 1649).
//
// Reference: src/map/entities/battle_entity.cpp GetSubWeaponDmg (~749–807)
//
// Host retains weapon-slot lookup, getMod / getMobMod, and production wire of
// CBattleEntity::GetSubWeaponDmg.
//
// Null sub-weapon note: LSB dereferences m_Weapons[SLOT_SUB] without a null check
// on the mob/pet branch (UB). Pure path injects weaponDamage; host should pass 0
// when the slot is empty so the clamp path still matches OmegaXI luascript host.

namespace subweapondmghelpers
{

// Entity-kind pins for ResolveSubWeaponDmg.
enum class EntityKind : std::uint8_t
{
    PC = 0,
    MobOrPetNonAutomaton,
    Other,
};

// TYPE_MOB or (TYPE_PET && not AUTOMATON) branch:
//   mult = dmgMult/100, or baseDamageMultiplier/100 when baseDamageMultiplier != 0
//   damage = floor((weaponDamage + baseDamageModifier) * mult)
//          + subDmgRating + damageOffset
//   clamp to [1, 65535]
//
// Non-mob pets on this branch never apply mob mods — inject dmgMult=100 and zeros.
// Missing sub weapon: inject weaponDamage=0 (LSB would null-deref).
inline auto ResolveMobOrPetSubWeaponDmg(const std::int32_t weaponDamage,
                                        const std::int32_t baseDamageModifier,
                                        const std::int32_t dmgMult,
                                        const std::int32_t baseDamageMultiplier,
                                        const std::int32_t subDmgRating,
                                        const std::int32_t damageOffset) -> std::uint16_t
{
    float mult = static_cast<float>(dmgMult) / 100.0f;
    if (baseDamageMultiplier != 0)
    {
        mult = static_cast<float>(baseDamageMultiplier) / 100.0f;
    }
    auto damage = static_cast<std::int32_t>(std::floor((weaponDamage + baseDamageModifier) * mult));
    damage += subDmgRating + damageOffset;
    damage = std::clamp(damage, 1, 65535);
    return static_cast<std::uint16_t>(damage);
}

// PC / other fall-through including automaton pets (hasWeapon false → 0).
// When isPC && reqLvl > mLevel:
//   scaled = weaponDmg * mLevel * 3 / 4 / reqLvl  (uint32 intermediates)
// return scaled + itemDmgRating + subDmgRating  (int32 sum → uint16)
inline auto ResolvePCSubWeaponDmg(const bool          hasWeapon,
                                  const std::uint16_t weaponDmg,
                                  const std::uint8_t  reqLvl,
                                  const std::uint8_t  mLevel,
                                  const bool          isPC,
                                  const std::int16_t  itemDmgRating,
                                  const std::int32_t  subDmgRating) -> std::uint16_t
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
    const auto sum = static_cast<std::int32_t>(base) + static_cast<std::int32_t>(itemDmgRating) + subDmgRating;
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
    std::int32_t  subDmgRating{ 0 };
    bool          hasWeapon{ false };
    std::uint16_t weaponDmg{ 0 };
    std::uint8_t  reqLvl{ 0 };
    std::uint8_t  mLevel{ 0 };
    std::int16_t  itemDmgRating{ 0 };
};

// Dispatcher mirroring GetSubWeaponDmg entity-kind branches.
inline auto ResolveSubWeaponDmg(const Params& p) -> std::uint16_t
{
    switch (p.kind)
    {
        case EntityKind::MobOrPetNonAutomaton:
            return ResolveMobOrPetSubWeaponDmg(p.weaponDamage, p.baseDamageModifier, p.dmgMult, p.baseDamageMultiplier, p.subDmgRating, p.damageOffset);
        case EntityKind::PC:
            return ResolvePCSubWeaponDmg(p.hasWeapon, p.weaponDmg, p.reqLvl, p.mLevel, true, p.itemDmgRating, p.subDmgRating);
        default: // Other (automaton, trust, …)
            return ResolvePCSubWeaponDmg(p.hasWeapon, p.weaponDmg, p.reqLvl, p.mLevel, false, p.itemDmgRating, p.subDmgRating);
    }
}

} // namespace subweapondmghelpers
