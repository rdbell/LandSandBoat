#pragma once

#include "map/mod_stat_capacity.h"

#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

// Pure CBattleEntity::addTrait / delTrait planning and mod-amount math.
// Parity: internal/traitmod (slice 1682).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::addTrait (~2280–2286)
//   CBattleEntity::delTrait (~2288–2294)
//
// Host retains TraitList (vector<CTrait*>), pointer-identity erase on del,
// and m_modStat mutation. LSB delTrait erases by CTrait* equality
// (remove-erase), not by trait ID. ID helpers are pure-test approximations.
// List membership checks stay in battletraithelpers::HasTrait (slice 1655).
//
// addTrait:
//   TraitList.emplace_back(PTrait);
//   addModifier(PTrait->getMod(), PTrait->getValue());
// delTrait:
//   delModifier(PTrait->getMod(), PTrait->getValue());
//   TraitList.erase(remove(... PTrait ...));

namespace traitmodhelpers
{

// TraitMod is the pure projection of a CTrait for add/del planning.
// TraitID ↔ getID(), ModID ↔ getMod() as uint16, Value ↔ getValue().
struct TraitMod
{
    std::uint16_t traitID{ 0 };
    std::uint16_t modID{ 0 };
    std::int16_t  value{ 0 };
};

// PlanAddTrait: projects append trait ID, mod ID, and add amount.
// Host still owns pointer storage; Mod::NONE still appends the trait while
// the mod map is a no-op via ApplyAddTraitMod.
inline auto PlanAddTrait(const TraitMod& t)
    -> std::tuple<std::uint16_t, std::uint16_t, std::int16_t>
{
    return { t.traitID, t.modID, t.value };
}

// PlanDelTrait: projects mod ID and del amount. Host erases CTrait* by
// pointer identity; pure code does not perform pointer erase.
inline auto PlanDelTrait(const TraitMod& t) -> std::pair<std::uint16_t, std::int16_t>
{
    return { t.modID, t.value };
}

// ApplyAddTraitMod: addTrait modifier delta. Same math as modstathelpers::ApplyAdd.
inline auto ApplyAddTraitMod(const std::int16_t current, const std::int16_t value, const std::uint16_t modID)
    -> std::int16_t
{
    return modstathelpers::ApplyAdd(current, value, modID);
}

// ApplyDelTraitMod: delTrait modifier delta. Same math as modstathelpers::ApplyDel.
inline auto ApplyDelTraitMod(const std::int16_t current, const std::int16_t value, const std::uint16_t modID)
    -> std::int16_t
{
    return modstathelpers::ApplyDel(current, value, modID);
}

// AppendTraitID: pure stand-in for TraitList.emplace_back ID projection.
// Does not dedupe — LSB allows multiple entries.
inline auto AppendTraitID(std::vector<std::uint16_t> ids, const std::uint16_t id)
    -> std::vector<std::uint16_t>
{
    ids.push_back(id);
    return ids;
}

// RemoveTraitID: first matching ID removal (pure-test approximation of
// pointer-identity erase). Absent id leaves the vector unchanged.
inline auto RemoveTraitID(std::vector<std::uint16_t> ids, const std::uint16_t id)
    -> std::vector<std::uint16_t>
{
    for (std::size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == id)
        {
            ids.erase(ids.begin() + static_cast<std::ptrdiff_t>(i));
            return ids;
        }
    }
    return ids;
}

} // namespace traitmodhelpers
