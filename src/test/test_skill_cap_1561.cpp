#include "test_skill_cap_1561.h"

#include "map/skill_cap_capacity.h"

#include <iostream>

namespace
{
using skillcaphelpers::CapLevelForSkillTable;
using skillcaphelpers::ClampSkillTableLevel;
using skillcaphelpers::LevelExceedsSkillTable;
using skillcaphelpers::ResolveSkillTableLevel;

auto Check() -> bool
{
    if (CapLevelForSkillTable(99) != 99 || CapLevelForSkillTable(100) != 99 || CapLevelForSkillTable(255) != 99)
    {
        return false;
    }
    if (CapLevelForSkillTable(0) != 0 || CapLevelForSkillTable(50) != 50)
    {
        return false;
    }

    // maxLevel = 99 for 100-row table
    if (ClampSkillTableLevel(50, 99) != 50 || ClampSkillTableLevel(99, 99) != 99 ||
        ClampSkillTableLevel(100, 99) != 99 || ClampSkillTableLevel(0, 99) != 0)
    {
        return false;
    }

    // skill overload: soft-cap then clamp
    if (ResolveSkillTableLevel(120, 99, true) != 99)
    {
        return false;
    }
    if (ResolveSkillTableLevel(50, 99, true) != 50)
    {
        return false;
    }
    // rank overload: no soft-cap — but clamp still applies
    // level 120 with maxLevel 99 → 99; soft cap off means 120 clamps to 99
    if (ResolveSkillTableLevel(120, 99, false) != 99)
    {
        return false;
    }

    if (!LevelExceedsSkillTable(100, 99) || LevelExceedsSkillTable(99, 99) || LevelExceedsSkillTable(50, 99))
    {
        return false;
    }

    return true;
}
} // namespace

auto runSkillCap1561SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "skill_cap_1561 self-tests failed\n";
        return false;
    }
    return true;
}
