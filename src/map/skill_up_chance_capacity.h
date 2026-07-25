#pragma once

#include "skill_up_capacity.h"

#include <cstdint>

namespace skillupchancehelpers
{
struct Facts
{
    uint8_t  skillID{};
    uint8_t  rank{};
    uint16_t workingSkill{};
    bool     useSubSkill{};
    uint16_t mainCapSkill{};
    uint16_t subCapSkill{};
    uint16_t mainMaxSkill{};
    uint16_t subMaxSkill{};
    uint16_t currentSkill{};
    double   chanceMultiplier{};
    int16_t  combatRateMod{};
    int16_t  magicRateMod{};
    double   random{};
    bool     forceSkillUp{};
};

struct Plan
{
    bool     considerSkillUp{};
    uint16_t capSkill{};
    uint16_t maxSkill{};
    int16_t  diff{};
    double   chance{};
    bool     gainSkillUp{};
};

inline auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{
        .considerSkillUp = skilluphelpers::ShouldConsiderSkillUp(facts.rank, facts.workingSkill, facts.useSubSkill),
    };
    if (!plan.considerSkillUp)
    {
        return plan;
    }

    plan.capSkill = skilluphelpers::ResolveCapSkill(facts.mainCapSkill, facts.subCapSkill, facts.useSubSkill);
    plan.maxSkill = skilluphelpers::ResolveMaxSkill(
        facts.mainCapSkill, facts.subCapSkill, facts.mainMaxSkill, facts.subMaxSkill, facts.useSubSkill);
    plan.diff   = skilluphelpers::SkillDiff(plan.maxSkill, facts.currentSkill);
    plan.chance = skilluphelpers::BaseSkillUpChance(plan.diff, facts.currentSkill, facts.chanceMultiplier);
    plan.chance = skilluphelpers::ClampSkillUpChance(plan.chance);
    // Rate modifiers intentionally apply after the base chance clamp.
    if (skilluphelpers::IsCombatSkillUpSkill(facts.skillID))
    {
        plan.chance = skilluphelpers::ApplySkillUpRateMod(plan.chance, facts.combatRateMod);
    }
    else if (skilluphelpers::IsMagicSkillUpSkill(facts.skillID))
    {
        plan.chance = skilluphelpers::ApplySkillUpRateMod(plan.chance, facts.magicRateMod);
    }
    plan.gainSkillUp = skilluphelpers::ShouldGainSkillUp(plan.diff, facts.random, plan.chance, facts.forceSkillUp);
    return plan;
}
} // namespace skillupchancehelpers
