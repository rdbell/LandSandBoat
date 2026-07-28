#include "test_monstrosity_monster_skill_7485.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityMonsterSkill7485SelfTests() -> bool
{
    using monstrosity::PlanMonsterSkillAction;

    const auto wrongJob = PlanMonsterSkillAction(JOB_WAR, true, 0x1234, 0x5678);
    if (wrongJob.invokeMobSkill)
    {
        std::cerr << "monstrosity monster skill: non-MON job failed\n";
        return false;
    }

    const auto absent = PlanMonsterSkillAction(JOB_MON, false, 0x1234, 0x5678);
    if (absent.invokeMobSkill)
    {
        std::cerr << "monstrosity monster skill: absent data failed\n";
        return false;
    }

    const auto active = PlanMonsterSkillAction(JOB_MON, true, 0x1234, 0x5678);
    if (!active.invokeMobSkill || active.actionIndex != 0x1234 || active.skillId != 0x5678)
    {
        std::cerr << "monstrosity monster skill: active dispatch failed\n";
        return false;
    }

    return true;
}
