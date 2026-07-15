#pragma once

#include <cstdint>
#include <unordered_map>

// Pure CBattleEntity::getMod with injected stored value or map snapshot.
// Parity: internal/getmod (slice 1679).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::getMod (~2116–2126)
//
// Host retains m_modStat map storage. Helpers take either a pre-resolved
// stored value or a map snapshot. Missing map keys resolve to 0 (matching
// unordered_map default-constructed int16 after operator[]).
//
// Kept independent from modstathelpers so readers and mutators stay separate.

namespace getmodhelpers
{

// Mod::NONE — essential sentinel that returns 0 from getMod.
constexpr std::uint16_t ModNone = 0;

// GetMod mirrors CBattleEntity::getMod with an injected stored value.
//
//   if modID == Mod::NONE → 0
//   return stored   // host-resolved m_modStat[modID]
//
// When modID is NONE the stored value is ignored. For non-NONE ids the host
// supplies the current m_modStat entry (0 when the key is absent).
inline auto GetMod(const std::uint16_t modID, const std::int16_t stored) -> std::int16_t
{
    if (modID == ModNone)
    {
        return 0;
    }
    return stored;
}

// GetModFromMap mirrors CBattleEntity::getMod against a map snapshot.
//
//   if modID == Mod::NONE → 0
//   return mods[modID] if present else 0
//
// An empty map yields 0 for non-NONE ids.
inline auto GetModFromMap(const std::uint16_t modID, const std::unordered_map<std::uint16_t, std::int16_t>& mods)
    -> std::int16_t
{
    if (modID == ModNone)
    {
        return 0;
    }
    const auto it = mods.find(modID);
    if (it == mods.end())
    {
        return 0;
    }
    return it->second;
}

} // namespace getmodhelpers
