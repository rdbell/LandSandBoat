#pragma once

#include "pet_ability_table_capacity.h"

#include <cstdint>
#include <vector>

namespace jugpetabilityrosterhelpers
{
struct Facts
{
    bool                  isJugPet{};
    std::vector<uint16_t> mobSkillIDs{};
};

struct Plan
{
    std::vector<uint16_t> addPetAbilityBits{};
};

inline auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan;
    if (!facts.isJugPet)
    {
        return plan;
    }
    for (const auto abilityID : facts.mobSkillIDs)
    {
        plan.addPetAbilityBits.push_back(petabilitytablehelpers::JugPetAbilityBit(abilityID));
    }
    return plan;
}
} // namespace jugpetabilityrosterhelpers
