#pragma once

#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

// Pure CBattleEntity::addModifiers (list path) amount math with fully injected
// maps. Parity: internal/addmodlist (slice 1692).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::addModifiers (~1819–1830)
//
// Host retains m_modStat map storage and list ownership. Helpers take injected
// current map and list entries (raw uint16 mod IDs + int16 amounts).
//
// Asymmetry with delmodlisthelpers::ApplyDelModifiers / CBattleEntity::delModifiers:
//   addModifiers(modList): for each entry { if id != NONE { m_modStat[id] += amount } }
//   delModifiers(modList): for each entry { m_modStat[id] -= amount }
//                          // NO NONE guard — ID 0 is still subtracted
//
// Alignment with modstathelpers::ApplyAdd / CBattleEntity::addModifier:
//   addModifier(type, amount): if type != NONE { m_modStat[type] += amount }
//   addModifiers list path:    same NONE skip per entry

namespace addmodlisthelpers
{

// ModNone is Mod::NONE — list entries with this ID are skipped.
constexpr std::uint16_t ModNone = 0;

// ModEntry is one CModifier-style list entry: raw Mod ID and signed amount.
struct ModEntry
{
    std::uint16_t id{};
    std::int16_t  amount{};
};

// ApplyAddToValue mirrors one addModifiers map mutation for a non-NONE entry:
//   current += amount
// Pure arithmetic only; callers must gate Mod::NONE before invoking.
// Contrast delmodlisthelpers::ApplyDelToValue, which always subtracts with no
// gate at either the value or list layer.
inline auto ApplyAddToValue(const std::int16_t current, const std::int16_t amount) -> std::int16_t
{
    return static_cast<std::int16_t>(current + amount);
}

// ApplyAddModifiers returns a new map equal to current with every non-NONE
// list entry amount added to the corresponding key:
//
//   for each entry in list:
//     if entry.id != NONE:
//       result[entry.id] += entry.amount
//
// The input map is not mutated. Missing keys resolve to 0 (LSB operator[]).
// Mod ID 0 (NONE) is skipped — matching live LSB addModifiers (and differing
// from delModifiers / delmodlisthelpers::ApplyDelModifiers, which still
// subtract ID 0).
//
// Keys present in current are always copied (including NONE if present).
// Non-NONE list entries introduce or update keys even when the resulting
// value is zero or negative. Multiple list entries with the same non-NONE
// ID accumulate sequential additions.
inline auto ApplyAddModifiers(const std::unordered_map<std::uint16_t, std::int16_t>& current,
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
        // Missing key: result[entry.id] default-constructs to 0, then +=.
        const auto it  = result.find(entry.id);
        const auto cur = (it == result.end()) ? static_cast<std::int16_t>(0) : it->second;
        result[entry.id] = ApplyAddToValue(cur, entry.amount);
    }
    return result;
}

// Convenience overload for vector-backed lists (mirrors std::vector<CModifier>*).
inline auto ApplyAddModifiers(const std::unordered_map<std::uint16_t, std::int16_t>& current,
                              const std::vector<ModEntry>&                           list)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    return ApplyAddModifiers(current, std::span<const ModEntry>{ list });
}

} // namespace addmodlisthelpers
