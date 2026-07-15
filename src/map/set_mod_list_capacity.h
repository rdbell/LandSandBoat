#pragma once

#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

// Pure CBattleEntity::setModifiers (list path) amount math with fully injected
// maps. Parity: internal/setmodlist (slice 1693).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::setModifiers (~1928–1939)
//
// Host retains m_modStat map storage and list ownership. Helpers take injected
// current map and list entries (raw uint16 mod IDs + int16 amounts).
//
// Symmetry with modstathelpers::ApplySet / CBattleEntity::setModifier:
//   setModifier(type, amount):  if type != NONE { m_modStat[type] = amount }
//   setModifiers(modList):      for each entry {
//                                 if id != NONE { m_modStat[id] = amount }
//                               }
//
// Contrast delModifiers (delmodlisthelpers), which has no NONE guard.

namespace setmodlisthelpers
{

// Mod::NONE — list entries with this ID are skipped.
inline constexpr std::uint16_t ModNone = 0;

// ModEntry is one CModifier-style list entry: raw Mod ID and signed amount.
struct ModEntry
{
    std::uint16_t id{};
    std::int16_t  amount{};
};

// ApplySetModifiers returns a new map equal to current with every non-NONE
// list entry amount assigned (set, not added) to the corresponding key:
//
//   for each entry in list:
//     if entry.id != NONE:
//       result[entry.id] = entry.amount
//
// The input map is not mutated (deep copy of keys present in current). NONE
// entries are skipped, matching live LSB setModifiers and single-path
// setModifier / modstathelpers::ApplySet.
//
// Keys present in current are always copied. List entries introduce or update
// keys even when the assigned value is zero or negative. Multiple non-NONE
// list entries with the same ID overwrite sequentially (last write wins).
inline auto ApplySetModifiers(const std::unordered_map<std::uint16_t, std::int16_t>& current,
                              const std::span<const ModEntry>                        list)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    std::unordered_map<std::uint16_t, std::int16_t> result = current;
    for (const auto& entry : list)
    {
        if (entry.id == ModNone)
        {
            continue;
        }
        result[entry.id] = entry.amount;
    }
    return result;
}

// Convenience overload for vector-backed lists (mirrors std::vector<CModifier>*).
inline auto ApplySetModifiers(const std::unordered_map<std::uint16_t, std::int16_t>& current,
                              const std::vector<ModEntry>&                           list)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    return ApplySetModifiers(current, std::span<const ModEntry>{ list });
}

} // namespace setmodlisthelpers
