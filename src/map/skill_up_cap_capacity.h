#pragma once

#include "skill_up_capacity.h"

#include <cstdint>

namespace skillupcaphelpers
{
struct Facts
{
    uint16_t currentSkill{};
    uint8_t  skillAmount{};
    uint16_t capSkill{};
};

struct Plan
{
    uint8_t skillAmount{};
    bool    markSkillCapped{};
};

constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{ .skillAmount = facts.skillAmount };
    if (skilluphelpers::HitsSkillCap(facts.skillAmount, facts.currentSkill, facts.capSkill))
    {
        plan.skillAmount     = skilluphelpers::CapSkillAmountToCeiling(facts.skillAmount, facts.currentSkill, facts.capSkill);
        plan.markSkillCapped = true;
    }
    return plan;
}
} // namespace skillupcaphelpers
