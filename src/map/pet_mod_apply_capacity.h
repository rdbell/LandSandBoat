#pragma once

#include <algorithm>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Pure CBattleEntity applyPetModifiers / removePetModifiers planning for
// m_petMod. Parity: internal/petmodapply (slice 1688).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::applyPetModifiers  (~2228–2243)
//   CBattleEntity::removePetModifiers (~2245–2260)
//
// Host retains CheckPetModType, live pet addModifier/delModifier, and
// UpdateHealth. Eligible pet-mod types are fully injected (host resolves
// CheckPetModType into a set of PetModType raw ids).
//
// Amounts are returned as stored: apply → addModifier; remove → delModifier
// (not negated). Stable order: petmod ascending, then modID ascending.

namespace petmodapplyhelpers
{

// Store is m_petMod with raw keys:
//   PetModType (uint8) → Mod (uint16) → amount (int16)
using Store = std::unordered_map<std::uint8_t, std::unordered_map<std::uint16_t, std::int16_t>>;

// ModDelta is one planned {modID, amount} row to apply to a live pet.
struct ModDelta
{
    std::uint16_t ModID{};
    std::int16_t  Amount{};
};

// Shared flatten/filter for apply and remove.
// Iterates store, keeps only eligible petmod keys, emits deltas sorted by
// petmod then modID. Amounts as stored.
inline auto planPetModifiers(const Store& store, const std::span<const std::uint8_t> eligiblePetModTypes)
    -> std::vector<ModDelta>
{
    if (store.empty() || eligiblePetModTypes.empty())
    {
        return {};
    }

    std::unordered_set<std::uint8_t> eligible(eligiblePetModTypes.begin(), eligiblePetModTypes.end());

    std::vector<std::uint8_t> petmods;
    petmods.reserve(store.size());
    for (const auto& [petmod, inner] : store)
    {
        if (eligible.find(petmod) == eligible.end())
        {
            continue;
        }
        if (inner.empty())
        {
            continue;
        }
        petmods.push_back(petmod);
    }
    if (petmods.empty())
    {
        return {};
    }
    std::sort(petmods.begin(), petmods.end());

    std::vector<ModDelta> out;
    for (const auto petmod : petmods)
    {
        const auto& inner = store.at(petmod);
        std::vector<std::uint16_t> modIDs;
        modIDs.reserve(inner.size());
        for (const auto& [modID, _] : inner)
        {
            modIDs.push_back(modID);
        }
        std::sort(modIDs.begin(), modIDs.end());
        for (const auto modID : modIDs)
        {
            out.push_back(ModDelta{ modID, inner.at(modID) });
        }
    }
    return out;
}

// PlanApplyPetModifiers flattens store into addModifier deltas for eligible
// pet-mod types. Amounts as stored.
//
// Host:
//   for (const auto& d : PlanApplyPetModifiers(store, eligible)) {
//       PPet->addModifier(static_cast<Mod>(d.ModID), d.Amount);
//       PPet->UpdateHealth();
//   }
inline auto PlanApplyPetModifiers(const Store& store, const std::span<const std::uint8_t> eligiblePetModTypes)
    -> std::vector<ModDelta>
{
    return planPetModifiers(store, eligiblePetModTypes);
}

// Convenience overload for vector-backed eligible lists.
inline auto PlanApplyPetModifiers(const Store& store, const std::vector<std::uint8_t>& eligiblePetModTypes)
    -> std::vector<ModDelta>
{
    return planPetModifiers(store, std::span<const std::uint8_t>{ eligiblePetModTypes });
}

// PlanRemovePetModifiers flattens store into delModifier deltas for eligible
// pet-mod types. Amounts as stored (not negated).
//
// Host:
//   for (const auto& d : PlanRemovePetModifiers(store, eligible)) {
//       PPet->delModifier(static_cast<Mod>(d.ModID), d.Amount);
//       PPet->UpdateHealth();
//   }
inline auto PlanRemovePetModifiers(const Store& store, const std::span<const std::uint8_t> eligiblePetModTypes)
    -> std::vector<ModDelta>
{
    return planPetModifiers(store, eligiblePetModTypes);
}

// Convenience overload for vector-backed eligible lists.
inline auto PlanRemovePetModifiers(const Store& store, const std::vector<std::uint8_t>& eligiblePetModTypes)
    -> std::vector<ModDelta>
{
    return planPetModifiers(store, std::span<const std::uint8_t>{ eligiblePetModTypes });
}

} // namespace petmodapplyhelpers
