#pragma once

#include <cstdint>
#include <unordered_map>

// Pure CBattleEntity addPetModifier / setPetModifier / delPetModifier nested
// map storage math for m_petMod. Parity: internal/petmodmap (slice 1684).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::addPetModifier (~2169–2180)
//   CBattleEntity::setPetModifier (~2182–2193)
//   CBattleEntity::delPetModifier  (~2195–2206)
//
// Host retains CheckPetModType, live pet modifier application, and
// UpdateHealth. Helpers take an injected nested store and return a new store
// (input is not mutated). Missing outer/inner keys auto-create like operator[].
//
// No Mod::NONE skip on this path (unlike modstathelpers single-mod math).

namespace petmodmaphelpers
{

// Store is m_petMod with raw keys:
//   PetModType (uint8) → Mod (uint16) → amount (int16)
using Store = std::unordered_map<std::uint8_t, std::unordered_map<std::uint16_t, std::int16_t>>;

// Deep-copy nested store so mutating the result cannot touch src.
inline auto CloneStore(const Store& src) -> Store
{
    Store out;
    out.reserve(src.size());
    for (const auto& [petmod, inner] : src)
    {
        out.emplace(petmod, inner); // unordered_map value-copy of nested map
    }
    return out;
}

// Read amount for (petmod, modID); missing keys → 0 (does not insert).
inline auto Get(const Store& store, const std::uint8_t petmod, const std::uint16_t modID) -> std::int16_t
{
    const auto outer = store.find(petmod);
    if (outer == store.end())
    {
        return 0;
    }
    const auto inner = outer->second.find(modID);
    if (inner == outer->second.end())
    {
        return 0;
    }
    return inner->second;
}

// ApplyAdd mirrors CBattleEntity::addPetModifier storage half:
//   m_petMod[petmod][type] += amount
inline auto ApplyAdd(const Store& store, const std::uint8_t petmod, const std::uint16_t modID, const std::int16_t amount)
    -> Store
{
    Store result = CloneStore(store);
    auto& inner  = result[petmod]; // auto-create outer
    const auto it  = inner.find(modID);
    const auto cur = (it == inner.end()) ? static_cast<std::int16_t>(0) : it->second;
    inner[modID]   = static_cast<std::int16_t>(cur + amount);
    return result;
}

// ApplySet mirrors CBattleEntity::setPetModifier storage half:
//   m_petMod[petmod][type] = amount
inline auto ApplySet(const Store& store, const std::uint8_t petmod, const std::uint16_t modID, const std::int16_t amount)
    -> Store
{
    Store result   = CloneStore(store);
    result[petmod][modID] = amount;
    return result;
}

// ApplyDel mirrors CBattleEntity::delPetModifier storage half:
//   m_petMod[petmod][type] -= amount
inline auto ApplyDel(const Store& store, const std::uint8_t petmod, const std::uint16_t modID, const std::int16_t amount)
    -> Store
{
    Store result = CloneStore(store);
    auto& inner  = result[petmod];
    const auto it  = inner.find(modID);
    const auto cur = (it == inner.end()) ? static_cast<std::int16_t>(0) : it->second;
    inner[modID]   = static_cast<std::int16_t>(cur - amount);
    return result;
}

} // namespace petmodmaphelpers
