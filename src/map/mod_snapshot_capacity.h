#pragma once

#include <cstdint>
#include <unordered_map>

// Pure CBattleEntity::saveModifiers / restoreModifiers map deep-copy.
// Parity: internal/modsnapshot (slice 1683).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::saveModifiers (~1955–1958)
//   CBattleEntity::restoreModifiers (~1960–1963)
//
// Host retains m_modStat / m_modStatSave ownership and assigns the returned
// maps. LSB production uses unordered_map assignment (value copy); these
// helpers make that copy explicit with fully injected maps so tests stay
// free of CBattleEntity.

namespace modsnapshothelpers
{

// deepCopyMods returns an independent map with the same key/value pairs as src.
// An empty src yields an empty map. Mutating the result must not affect src.
inline auto deepCopyMods(const std::unordered_map<std::uint16_t, std::int16_t>& src)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    // unordered_map copy construction is value/deep copy of elements.
    return std::unordered_map<std::uint16_t, std::int16_t>{ src };
}

// SnapshotMods mirrors CBattleEntity::saveModifiers:
//   m_modStatSave = m_modStat
//
// Returns a deep copy of src. Mutating the result must not affect src.
inline auto SnapshotMods(const std::unordered_map<std::uint16_t, std::int16_t>& src)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    return deepCopyMods(src);
}

// RestoreMods mirrors CBattleEntity::restoreModifiers:
//   m_modStat = m_modStatSave
//
// Returns a deep copy of saved. An empty saved yields an empty map (clears all
// current mods when assigned). Mutating the result must not affect saved.
inline auto RestoreMods(const std::unordered_map<std::uint16_t, std::int16_t>& saved)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    return deepCopyMods(saved);
}

} // namespace modsnapshothelpers
