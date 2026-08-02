#include "test_building_skills_1509.h"

#include "map/building_skills_capacity.h"

#include <iostream>

namespace
{
using buildingskillshelpers::CapCurrentSkill;
using buildingskillshelpers::CraftWorkingSkill;
using buildingskillshelpers::IsAutomatonSkill;
using buildingskillshelpers::IsCombatSkillLoopID;
using buildingskillshelpers::IsCraftSkillLoopID;
using buildingskillshelpers::IsMainJobSkillPath;
using buildingskillshelpers::IsNonJobSkillPath;
using buildingskillshelpers::IsSubJobSkillPath;
using buildingskillshelpers::IsUnusedCombatSkillSlot;
using buildingskillshelpers::IsUnusedSkillLoopID;
using buildingskillshelpers::NonJobSkillWorkingValue;
using buildingskillshelpers::RealSkillLevels;
using buildingskillshelpers::ResolveRealSkillRank;
using buildingskillshelpers::ShouldSetBlueCapFlag;
using buildingskillshelpers::ShouldSetCraftBlueFlag;
using buildingskillshelpers::SkillModID;
using buildingskillshelpers::WithBlueFlag;
using buildingskillshelpers::WorkingSkillFromCurrentAndBonus;

auto Check() -> bool
{
    if (!IsUnusedCombatSkillSlot(13) || !IsUnusedCombatSkillSlot(21) || !IsUnusedCombatSkillSlot(46) || IsUnusedCombatSkillSlot(12))
    {
        return false;
    }
    if (!IsAutomatonSkill(22) || !IsAutomatonSkill(24) || IsAutomatonSkill(21) || IsAutomatonSkill(25))
    {
        return false;
    }
    if (ResolveRealSkillRank(3, 1) != 3 || ResolveRealSkillRank(0, 2) != 2)
    {
        return false;
    }
    if (CapCurrentSkill(120, 100) != 100 || CapCurrentSkill(80, 100) != 80)
    {
        return false;
    }
    if (WorkingSkillFromCurrentAndBonus(50, 10) != 60 || WorkingSkillFromCurrentAndBonus(5, -10) != 0)
    {
        return false;
    }
    // int16 narrowing: 300 + 0x7FFF → sum 33067 → int16 -32469 → 0
    if (WorkingSkillFromCurrentAndBonus(300, 0x7FFF) != 0)
    {
        return false;
    }
    if (!ShouldSetBlueCapFlag(100, 100) || ShouldSetBlueCapFlag(99, 100))
    {
        return false;
    }
    if (WithBlueFlag(60) != (60 | 0x8000))
    {
        return false;
    }
    if (NonJobSkillWorkingValue(15) != (15 | 0x8000) || NonJobSkillWorkingValue(-3) != 0x8000)
    {
        return false;
    }
    if (CraftWorkingSkill(250, 3) != (25 * 0x20 + 3))
    {
        return false;
    }
    if (!ShouldSetCraftBlueFlag(2, 300) || ShouldSetCraftBlueFlag(2, 299) || ShouldSetCraftBlueFlag(255, 100))
    {
        return false;
    }
    if (!IsCombatSkillLoopID(1) || !IsCombatSkillLoopID(47) || IsCombatSkillLoopID(48))
    {
        return false;
    }
    if (!IsCraftSkillLoopID(48) || !IsCraftSkillLoopID(57) || IsCraftSkillLoopID(58))
    {
        return false;
    }
    if (!IsUnusedSkillLoopID(58) || !IsUnusedSkillLoopID(63) || IsUnusedSkillLoopID(57))
    {
        return false;
    }
    if (!IsMainJobSkillPath(10) || IsMainJobSkillPath(0))
    {
        return false;
    }
    if (!IsSubJobSkillPath(0, 5) || IsSubJobSkillPath(5, 5) || !IsNonJobSkillPath(0, 0))
    {
        return false;
    }
    if (SkillModID(1) != 80 || RealSkillLevels(255) != 25)
    {
        return false;
    }
    return true;
}
} // namespace

auto runBuildingSkills1509SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "building skills 1509 self-test failed\n";
    }
    return ok;
}
