#pragma once

#include "skill_up_capacity.h"

#include <cstdint>

namespace skillupawardhelpers
{
struct Facts
{
    uint16_t currentSkill{};
    uint8_t  skillAmount{};
    bool     artsActive{};
    int16_t  skillBonusBefore{};
    int16_t  skillBonusAfter{};
};

struct Plan
{
    bool     sendSkillGain{};
    bool     incrementWorkingSkill{};
    bool     sendStatus{};
    bool     sendSkillLevelUp{};
    uint16_t skillLevel{};
    bool     checkWeaponSkill{};
    bool     saveSkill{};
};

constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{
        .sendSkillGain = true,
        .saveSkill     = true,
    };
    if (!skilluphelpers::CrossedSkillLevel(facts.currentSkill, facts.skillAmount))
    {
        return plan;
    }
    plan.incrementWorkingSkill = skilluphelpers::ShouldIncrementWorkingSkill(facts.artsActive, facts.skillBonusBefore, facts.skillBonusAfter);
    plan.sendStatus            = true;
    plan.sendSkillLevelUp      = true;
    plan.skillLevel            = static_cast<uint16_t>((facts.currentSkill + facts.skillAmount) / 10);
    plan.checkWeaponSkill      = true;
    return plan;
}
} // namespace skillupawardhelpers
