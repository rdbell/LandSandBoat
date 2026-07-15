#pragma once

#include <cstdint>
#include <vector>

// Pure CBattleEntity::hasTrait with fully injected TraitList IDs.
// Parity: internal/battletrait (slice 1655).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::hasTrait (~2296–2307)
//
// Host retains TraitList (vector<CTrait*>) and projects each entry via
// CTrait::getID() before calling the pure helper. This is the battle-entity
// TraitList scan path, not charutils::hasTrait (PC m_TraitList bitmask).

namespace battletraithelpers
{

// HasTrait mirrors CBattleEntity::hasTrait.
// traitIDs is the host-projected TraitList (each CTrait::getID()).
// want is the requested traitID.
// Empty traitIDs is a miss (false). First exact match returns true.
inline auto HasTrait(const std::vector<std::uint16_t>& traitIDs, const std::uint16_t want) -> bool
{
    for (const auto id : traitIDs)
    {
        if (id == want)
        {
            return true;
        }
    }
    return false;
}

} // namespace battletraithelpers
