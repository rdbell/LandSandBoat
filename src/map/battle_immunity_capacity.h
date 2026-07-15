#pragma once

#include <cstdint>

// Pure CBattleEntity::hasImmunity with fully injected inputs.
// Parity: internal/battleimmunity (slice 1646).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::hasImmunity (~240–248)
//
// Host retains m_Immunity storage and objtype; helpers take injected values
// only. LSB returns (m_Immunity & imID) as bool (any overlapping bits).

namespace battleimmunityhelpers
{

// ENTITYTYPE pins (baseentity.h).
constexpr std::uint8_t TypePC     = 0x01; // TYPE_PC
constexpr std::uint8_t TypeNPC    = 0x02; // TYPE_NPC
constexpr std::uint8_t TypeMob    = 0x04; // TYPE_MOB
constexpr std::uint8_t TypePet    = 0x08; // TYPE_PET
constexpr std::uint8_t TypeShip   = 0x10; // TYPE_SHIP
constexpr std::uint8_t TypeTrust  = 0x20; // TYPE_TRUST
constexpr std::uint8_t TypeFellow = 0x40; // TYPE_FELLOW

// IsMobOrPet mirrors the hasImmunity gate:
//   objtype == TYPE_MOB || objtype == TYPE_PET
inline auto IsMobOrPet(const std::uint8_t objtype) -> bool
{
    return objtype == TypeMob || objtype == TypePet;
}

// HasImmunity mirrors CBattleEntity::hasImmunity.
// immunityMask is m_Immunity; imID is the requested IMMUNITY bit(s).
// Non-zero AND is true (partial multi-bit overlap counts).
inline auto HasImmunity(const std::uint8_t  objtype,
                        const std::uint32_t immunityMask,
                        const std::uint32_t imID) -> bool
{
    if (!IsMobOrPet(objtype))
    {
        return false;
    }
    return (immunityMask & imID) != 0;
}

} // namespace battleimmunityhelpers
