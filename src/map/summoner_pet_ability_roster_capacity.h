#pragma once

#include "pet_ability_table_capacity.h"

#include <cstdint>
#include <vector>

namespace summonerpetabilityrosterhelpers
{
struct Candidate
{
    uint16_t id{};
    uint8_t  level{};
    bool     addTypeOK{};
};

struct Facts
{
    bool                   isSummoner{};
    uint8_t                petLevel{};
    uint32_t               petID{};
    std::vector<Candidate> abilities{};
};

struct Plan
{
    std::vector<uint16_t> addPetAbilityBits{};
};

inline auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan;
    if (!facts.isSummoner)
    {
        return plan;
    }
    for (const auto& ability : facts.abilities)
    {
        if (!petabilitytablehelpers::ShouldConsiderSMNPetAbility(facts.petLevel, ability.level, facts.petID, ability.addTypeOK))
        {
            continue;
        }
        if (facts.petID == petabilitytablehelpers::PetIDCarbuncle && petabilitytablehelpers::IsCarbuncleAbility(ability.id))
        {
            plan.addPetAbilityBits.push_back(petabilitytablehelpers::CarbunclePetAbilityBit(ability.id));
        }
        else if (petabilitytablehelpers::IsElementalAvatarPet(facts.petID) && petabilitytablehelpers::ElementalAvatarAbilityInBand(ability.id, facts.petID))
        {
            plan.addPetAbilityBits.push_back(petabilitytablehelpers::AvatarPetAbilityBit(ability.id));
        }
        else if (facts.petID == petabilitytablehelpers::PetIDDiabolos && petabilitytablehelpers::IsDiabolosAbility(ability.id))
        {
            plan.addPetAbilityBits.push_back(petabilitytablehelpers::AvatarPetAbilityBit(ability.id));
        }
        else if (facts.petID == petabilitytablehelpers::PetIDCaitSith && petabilitytablehelpers::IsCaitSithAbility(ability.id))
        {
            plan.addPetAbilityBits.push_back(petabilitytablehelpers::AvatarPetAbilityBit(ability.id));
        }
        else if (facts.petID == petabilitytablehelpers::PetIDSiren && petabilitytablehelpers::IsSirenAbility(ability.id))
        {
            plan.addPetAbilityBits.push_back(petabilitytablehelpers::SirenPetAbilityBit(ability.id));
        }
    }
    return plan;
}
} // namespace summonerpetabilityrosterhelpers
