#pragma once

#include <cstdint>

// Pure CBattleEntity::hasImmunity with fully injected inputs.
// Parity: internal/battleimmunity (slice 1646 residual / 3064 dual-wire).
//
// Dual-wire index:
//   - 1646: HasImmunity pure port + residual suite (test_battle_immunity_1646)
//   - 1712: production host wire (BaseEntity injects objType + immunityMask)
//   - 3064: HasImmunity dedicated dual-wire (has_immunity.go;
//           test_battle_immunity_has_3064)
//
// Formula (slice 3064 dual-wire; residual pure 1646 — formula unchanged):
//
//   HasImmunity(objtype, immunityMask, imID) =
//     if !IsMobOrPet(objtype): false
//     else (immunityMask & imID) != 0
//
// IsMobOrPet (sibling residual):
//   objtype == TypeMob(0x04) || objtype == TypePet(0x08)
//
// Host retains m_Immunity storage and objtype; helpers take injected values
// only. LSB returns (m_Immunity & imID) as bool (any overlapping bits).
// Go dual-wire: battleimmunity.HasImmunity (internal/battleimmunity/has_immunity.go).
// Future / existing host injects battleimmunityhelpers::HasImmunity(objtype,
// m_Immunity, imID) then status application path.
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::hasImmunity (~240–248)

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

// ---------------------------------------------------------------------------
// Sibling residual — IsMobOrPet exact type gate
// ---------------------------------------------------------------------------

// IsMobOrPet mirrors the hasImmunity gate:
//   objtype == TYPE_MOB || objtype == TYPE_PET
// Exact equality only (combined TypeMob|TypePet is false).
// Residual sibling used by HasImmunity dual-wire (slice 3064).
inline auto IsMobOrPet(const std::uint8_t objtype) -> bool
{
    return objtype == TypeMob || objtype == TypePet;
}

// ---------------------------------------------------------------------------
// Slice 1646 / 3064 — HasImmunity free-function dual-wire
// ---------------------------------------------------------------------------

// HasImmunity mirrors CBattleEntity::hasImmunity.
// immunityMask is m_Immunity; imID is the requested IMMUNITY bit(s).
// Non-zero AND is true (partial multi-bit overlap counts).
//
// Formula (slice 3064 dual-wire; residual pure 1646 — formula unchanged):
//   HasImmunity(objtype, immunityMask, imID) =
//     if !IsMobOrPet(objtype): false
//     else (immunityMask & imID) != 0
//
// Dual-wire of Go battleimmunity.HasImmunity (has_immunity.go).
// Call site: host injects objtype + m_Immunity + imID (slice 1712 BaseEntity
// path / future CBattleEntity::hasImmunity rewire).
// Prior pure port: slice 1646. Residual suite: test_battle_immunity_1646.
// Dedicated dual-wire suite: test_battle_immunity_has_3064.
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
