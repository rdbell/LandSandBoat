#pragma once

#include <cstdint>

// Pure DEATH_TYPE pins and Get/Set pass-through helpers.
// Parity: internal/deathtype (slice 1685).
//
// Reference: src/map/entities/battle_entity.h
//   enum class DEATH_TYPE : uint8 {
//     NONE=0, PHYSICAL=1, MAGICAL=2, WS_PHYSICAL=3, WS_MAGICAL=4
//   };
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::SetDeathType (~1795–1798)
//   CBattleEntity::GetDeathType (~1800–1803)
//
// Host retains m_DeathType storage. Helpers do not own entity state:
// Set returns the byte the host should store; Get returns the stored byte.
// LSB accepts any uint8 via cast; values outside the named catalog still
// round-trip. IsKnownDeathType is optional catalog membership (0..4).

namespace deathtypehelpers
{

// DEATH_TYPE enum class : uint8 pins.
constexpr std::uint8_t DeathTypeNone = 0; // DEATH_TYPE::NONE
constexpr std::uint8_t Physical      = 1; // DEATH_TYPE::PHYSICAL
constexpr std::uint8_t Magical       = 2; // DEATH_TYPE::MAGICAL
constexpr std::uint8_t WSPhysical    = 3; // DEATH_TYPE::WS_PHYSICAL
constexpr std::uint8_t WSMagical     = 4; // DEATH_TYPE::WS_MAGICAL

// Inclusive upper bound of named DEATH_TYPE values.
constexpr std::uint8_t MaxKnownDeathType = WSMagical;

// SetDeathType mirrors CBattleEntity::SetDeathType storage cast:
//   m_DeathType = static_cast<DEATH_TYPE>(type);
// Returns the byte the host should store. No range check.
constexpr auto SetDeathType(const std::uint8_t deathType) -> std::uint8_t
{
    return deathType;
}

// GetDeathType mirrors CBattleEntity::GetDeathType load cast:
//   return static_cast<uint8>(m_DeathType);
// Identity pass-through of the host-held underlying byte.
constexpr auto GetDeathType(const std::uint8_t stored) -> std::uint8_t
{
    return stored;
}

// IsKnownDeathType reports named catalog membership (NONE..WS_MAGICAL, 0..4).
// Optional; Set/Get do not require it.
constexpr auto IsKnownDeathType(const std::uint8_t deathType) -> bool
{
    return deathType <= MaxKnownDeathType;
}

} // namespace deathtypehelpers
