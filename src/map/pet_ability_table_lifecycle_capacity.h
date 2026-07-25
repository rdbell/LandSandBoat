#pragma once

#include "pet_ability_table_capacity.h"

#include <cstdint>

namespace petabilitytablelifecyclehelpers
{
struct Facts
{
    bool     petNull{};
    bool     charNull{};
    uint32_t petID{};
};

struct Plan
{
    bool reject{};
    bool clearPetCommands{};
    bool buildAbilityRosters{};
    bool pushCommandPacket{};
};

constexpr auto PlanFor(const Facts& facts) -> Plan
{
    if (petabilitytablehelpers::ShouldRejectNullPetOrChar(facts.petNull, facts.charNull))
    {
        return { .reject = true };
    }
    return {
        .clearPetCommands    = true,
        .buildAbilityRosters = !petabilitytablehelpers::ShouldClearPetCommandsOnly(facts.petID),
        .pushCommandPacket   = true,
    };
}
} // namespace petabilitytablelifecyclehelpers
