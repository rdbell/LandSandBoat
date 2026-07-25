#pragma once

#include "skill_up_capacity.h"

#include <cstdint>

namespace skillupextrastephelpers
{
struct Facts
{
    uint8_t tier{};
    uint8_t skillAmount{};
    double  random{};
};

struct Plan
{
    double  chance{};
    bool    stop{};
    uint8_t nextTier{};
    uint8_t nextSkillAmount{};
};

inline auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{
        .chance          = skilluphelpers::ExtraSkillUpTierChance(facts.tier),
        .nextTier        = facts.tier,
        .nextSkillAmount = facts.skillAmount,
    };
    plan.stop = skilluphelpers::ShouldStopExtraSkillUp(plan.chance, facts.random, facts.skillAmount);
    if (!plan.stop)
    {
        plan.nextTier -= 1;
        plan.nextSkillAmount += 1;
    }
    return plan;
}
} // namespace skillupextrastephelpers
