#pragma once

#include "pet_ability_table_capacity.h"

#include <cstdint>

namespace traitssourcehelpers
{
struct Facts
{
    uint8_t mainJob{};
    uint8_t subJob{};
    uint8_t mainLevel{};
    uint8_t subLevel{};

    bool    hasMonstrosity{};
    uint8_t monstrosityMainJob{};
    uint8_t monstrositySubJob{};
    uint8_t monstrosityLevel{};
};

struct Plan
{
    uint8_t mainJob{};
    uint8_t subJob{};
    uint8_t mainLevel{};
    uint8_t subLevel{};
    bool    calculateBlueTraits{};
};

constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{
        .mainJob   = facts.mainJob,
        .subJob    = facts.subJob,
        .mainLevel = facts.mainLevel,
        .subLevel  = facts.subLevel,
    };
    if (petabilitytablehelpers::ShouldApplyMonstrosityJobs(facts.hasMonstrosity))
    {
        plan.mainJob   = facts.monstrosityMainJob;
        plan.subJob    = facts.monstrositySubJob;
        plan.mainLevel = facts.monstrosityLevel;
        plan.subLevel  = facts.monstrosityLevel;
    }
    plan.calculateBlueTraits = petabilitytablehelpers::ShouldCalculateBlueTraits(plan.mainJob, plan.subJob);
    return plan;
}
} // namespace traitssourcehelpers
