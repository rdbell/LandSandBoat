#pragma once

#include <cstdint>

// Pure weapon-rank formulas from
// CBattleEntity::GetMainWeaponRank / GetSubWeaponRank / GetRangedWeaponRank.
// Parity: internal/weaponrank (slice 1645).
//
// Reference: src/map/entities/battle_entity.cpp ~917–976
//
// Host retains weapon-slot lookup, GetMainWeaponDmg / GetSubWeaponDmg, and
// item-modifier access; pure helpers take injected values only.

namespace weaponrankhelpers
{

// ResolveMainWeaponRank mirrors CBattleEntity::GetMainWeaponRank.
// mainWeaponDmg is the already-resolved GetMainWeaponDmg() value.
// When hasWeapon, item MAIN_DMG_RANK is added and item DMG_RATING subtracted.
// PC + H2H adds +3. Final result is integer wDamage / 9.
// Mod adjustments use uint16 wrap arithmetic matching LSB uint16 +=/-= getModifier().
inline auto ResolveMainWeaponRank(const std::uint16_t mainWeaponDmg,
                                  const bool          hasWeapon,
                                  const std::int16_t  mainDmgRank,
                                  const std::int16_t  dmgRating,
                                  const bool          isPC,
                                  const bool          isH2H) -> std::uint16_t
{
    auto wDamage = mainWeaponDmg;
    if (hasWeapon)
    {
        wDamage += static_cast<std::uint16_t>(mainDmgRank);
        wDamage -= static_cast<std::uint16_t>(dmgRating);
        if (isPC && isH2H)
        {
            wDamage += 3;
        }
    }
    return static_cast<std::uint16_t>(wDamage / 9);
}

// ResolveSubWeaponRank mirrors CBattleEntity::GetSubWeaponRank.
// Uses MAIN_DMG_RANK (yes MAIN, not SUB) and DMG_RATING; no H2H +3.
inline auto ResolveSubWeaponRank(const std::uint16_t subWeaponDmg,
                                 const bool          hasWeapon,
                                 const std::int16_t  mainDmgRank,
                                 const std::int16_t  dmgRating) -> std::uint16_t
{
    auto wDamage = subWeaponDmg;
    if (hasWeapon)
    {
        wDamage += static_cast<std::uint16_t>(mainDmgRank);
        wDamage -= static_cast<std::uint16_t>(dmgRating);
    }
    return static_cast<std::uint16_t>(wDamage / 9);
}

// ResolveRangedBaseDamage: PC underleveled scale of a single slot's damage.
// When isPC && reqLvl > mLevel:
//   scaled = baseDmg * mLevel * 3 / 4 / reqLvl  (integer, left-to-right)
// Intermediate product uses uint32 (matches OmegaXI host; avoids rare uint16 wrap).
// Otherwise returns baseDmg unchanged.
inline auto ResolveRangedBaseDamage(const std::uint16_t baseDmg,
                                    const std::uint8_t  reqLvl,
                                    const std::uint8_t  mLevel,
                                    const bool          isPC) -> std::uint16_t
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

// ResolveRangedWeaponRank mirrors CBattleEntity::GetRangedWeaponRank.
// Single-slot damage (ranged or ammo) + RANGED_DMG_RANK, not combined
// GetRangedWeaponDmg. No item DMG_RATING. Empty weapon → 0.
inline auto ResolveRangedWeaponRank(const bool          hasWeapon,
                                    const std::uint16_t baseDmg,
                                    const std::uint8_t  reqLvl,
                                    const std::uint8_t  mLevel,
                                    const bool          isPC,
                                    const std::int16_t  rangedDmgRank) -> std::uint16_t
{
    if (!hasWeapon)
    {
        return 0;
    }
    auto wDamage = ResolveRangedBaseDamage(baseDmg, reqLvl, mLevel, isPC);
    wDamage += static_cast<std::uint16_t>(rangedDmgRank);
    return static_cast<std::uint16_t>(wDamage / 9);
}

} // namespace weaponrankhelpers
