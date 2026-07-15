#pragma once

#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

// Pure CBattleEntity::delModifiers (list path) amount math with fully injected
// maps. Parity: internal/delmodlist (slice 1681).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::delModifiers (~2018–2026)
//
// Host retains m_modStat map storage and list ownership. Helpers take injected
// current map and list entries (raw uint16 mod IDs + int16 amounts).
//
// Asymmetry with modstathelpers::ApplyDel / CBattleEntity::delModifier:
//   delModifier(type, amount):  if type != NONE { m_modStat[type] -= amount }
//   delModifiers(modList):      for each entry { m_modStat[id] -= amount }
//                               // NO NONE guard — ID 0 is still subtracted

namespace delmodlisthelpers
{

// ModEntry is one CModifier-style list entry: raw Mod ID and signed amount.
struct ModEntry
{
    std::uint16_t id{};
    std::int16_t  amount{};
};

// ApplyDelToValue mirrors one delModifiers map mutation for a single entry:
//   current -= amount
// Always subtracts — no Mod::NONE guard. Contrast modstathelpers::ApplyDel,
// which returns current unchanged when modType is NONE.
inline auto ApplyDelToValue(const std::int16_t current, const std::int16_t amount) -> std::int16_t
{
    return static_cast<std::int16_t>(current - amount);
}

// ApplyDelModifiers returns a new map equal to current with every list entry
// amount subtracted from the corresponding key:
//
//   for each entry in list:
//     result[entry.id] -= entry.amount
//
// The input map is not mutated. Missing keys resolve to 0 (LSB operator[]).
// Mod ID 0 (NONE) is still applied — there is no skip, matching live LSB
// delModifiers (and differing from delModifier / modstathelpers::ApplyDel).
//
// Keys present in current are always copied. List entries introduce or update
// keys even when the resulting value is zero or negative. Multiple list
// entries with the same ID accumulate sequential subtractions.
inline auto ApplyDelModifiers(const std::unordered_map<std::uint16_t, std::int16_t>& current,
                              const std::span<const ModEntry>                        list)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    std::unordered_map<std::uint16_t, std::int16_t> result = current;
    for (const auto& entry : list)
    {
        // Missing key: result[entry.id] default-constructs to 0, then -=.
        // Use find/insert style so we still always write after subtract.
        const auto it  = result.find(entry.id);
        const auto cur = (it == result.end()) ? static_cast<std::int16_t>(0) : it->second;
        result[entry.id] = ApplyDelToValue(cur, entry.amount);
    }
    return result;
}

// Convenience overload for vector-backed lists (mirrors std::vector<CModifier>*).
inline auto ApplyDelModifiers(const std::unordered_map<std::uint16_t, std::int16_t>& current,
                              const std::vector<ModEntry>&                           list)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    return ApplyDelModifiers(current, std::span<const ModEntry>{ list });
}

} // namespace delmodlisthelpers
