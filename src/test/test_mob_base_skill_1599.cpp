#include "test_mob_base_skill_1599.h"

#include "map/mob_base_skill_capacity.h"

#include <iostream>

namespace
{
using namespace mobbaseskillhelpers;

auto Check() -> bool
{
    std::uint16_t skill = 0;
    std::uint8_t  job   = 0;
    if (!BaseSkillProxy(1, skill, job) || skill != SkillGreatAxe || job != JobWAR)
    {
        return false;
    }
    if (!BaseSkillProxy(5, skill, job) || skill != SkillThrowing || job != JobMNK)
    {
        return false;
    }
    if (!BaseSkillProxy(2, skill, job) || skill != SkillStaff || job != JobWAR ||
        !BaseSkillProxy(3, skill, job) || skill != SkillEvasion || job != JobWAR ||
        !BaseSkillProxy(4, skill, job) || skill != SkillArchery || job != JobWAR)
    {
        return false;
    }
    skill = 999;
    job   = 99;
    if (BaseSkillProxy(0, skill, job) || skill != 999 || job != 99 || BaseSkillProxy(6, skill, job) ||
        skill != 999 || job != 99 || BaseSkillProxy(255, skill, job) || skill != 999 || job != 99)
    {
        return false;
    }
    if (MagicEvasionCapRank(true) != 12 || MagicEvasionCapRank(false) != 7)
    {
        return false;
    }
    if (CapMagicEvasionLevel(0) != 0 || CapMagicEvasionLevel(50) != 50 || CapMagicEvasionLevel(99) != 99 ||
        CapMagicEvasionLevel(100) != 99 || CapMagicEvasionLevel(255) != 99)
    {
        return false;
    }
    if (JobSkillRankToBaseEvaRank(1) != 1 || JobSkillRankToBaseEvaRank(3) != 2 || JobSkillRankToBaseEvaRank(6) != 3 ||
        JobSkillRankToBaseEvaRank(9) != 4 || JobSkillRankToBaseEvaRank(10) != 5 || JobSkillRankToBaseEvaRank(0) != 0)
    {
        return false;
    }
    if (BestEvasionSkillRank(5, 2, false) != 2 || BestEvasionSkillRank(5, 2, true) != 5 ||
        BestEvasionSkillRank(2, 5, false) != 2 || BestEvasionSkillRank(4, 4, false) != 4 ||
        BestEvasionSkillRank(0, 255, true) != 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runMobBaseSkill1599SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "mob_base_skill_1599 self-tests failed\n";
        return false;
    }
    return true;
}
