#include "test_def_eva_1638.h"

#include "map/def_eva_capacity.h"

#include <iostream>

namespace
{
using namespace defevahelpers;

auto Check() -> bool
{
    // PC level DEF bands
    if (PCLevelDEFBonus(1) != 1 || PCLevelDEFBonus(50) != 50)
    {
        return false;
    }
    if (PCLevelDEFBonus(51) != 60 || PCLevelDEFBonus(60) != 78)
    {
        return false;
    }
    if (PCLevelDEFBonus(61) != 79 || PCLevelDEFBonus(90) != 108)
    {
        return false;
    }
    if (PCLevelDEFBonus(91) != 110 || PCLevelDEFBonus(99) != 122)
    {
        return false;
    }

    // Mob vitFactor 0.5 vs ally 1.5 (no PC level / mods)
    // 8 + floor(100 * 0.5) = 58; 8 + floor(100 * 1.5) = 158
    if (ResolveDEF(100, MobVitDefMultiplier, false, 0, 0, 0, 0, 0, false) != 58)
    {
        return false;
    }
    if (ResolveDEF(100, DefaultPlayerAlliesVitDefMultiplier, false, 0, 0, 0, 0, 0, false) != 158)
    {
        return false;
    }

    // PC level + DEF mod: 8 + 150 + 50 + 20 = 228
    if (ResolveDEF(100, DefaultPlayerAlliesVitDefMultiplier, true, 50, 20, 0, 0, 0, false) != 228)
    {
        return false;
    }
    // Non-PC ignores level: 8 + 150 + 20 = 178
    if (ResolveDEF(100, DefaultPlayerAlliesVitDefMultiplier, false, 50, 20, 0, 0, 0, false) != 178)
    {
        return false;
    }

    // Counterstance halves after mods; skips DEFP/food and max(1, …)
    // DEF = 8 + 100 = 108 → 54
    if (ResolveDEF(0, 1.5f, false, 0, 100, 50, 20, 5, true) != 54)
    {
        return false;
    }

    // Food DEF cap via min(): DEF=100, foodDefP=20, cap=5 → 100 + 0 + 5 = 105
    if (ResolveDEF(0, 1.5f, false, 0, 92, 0, 20, 5, false) != 105)
    {
        return false;
    }
    // Under cap: min(10, 50) = 10 → 110
    if (ResolveDEF(0, 1.5f, false, 0, 92, 0, 10, 50, false) != 110)
    {
        return false;
    }
    // DEFP + food: 100 + 50 + 5 = 155
    if (ResolveDEF(0, 1.5f, false, 0, 92, 50, 20, 5, false) != 155)
    {
        return false;
    }

    // EVA skill soft curve at 200 / 250
    if (SkillEvasionFromSkill(200) != 200 || SkillEvasionFromSkill(250) != 245)
    {
        return false;
    }
    // 201: 200 + 0.9 → truncate 200
    if (SkillEvasionFromSkill(201) != 200 || SkillEvasionFromSkill(100) != 100)
    {
        return false;
    }

    // AGI/2 contribution (integer division)
    if (ResolveEVA(50, 0, 0, false) != 25 || ResolveEVA(51, 0, 0, false) != 25)
    {
        return false;
    }

    // Mob vs PC EVA mod placement
    // Mob: base=evaMod 100 + AGI/2 25, no terminal mod → 125
    if (ResolveEVA(50, 999, 100, true) != 125)
    {
        return false;
    }
    // PC: skill 100 + AGI/2 25 + evaMod 30 → 155
    if (ResolveEVA(50, 100, 30, false) != 155)
    {
        return false;
    }
    // Soft curve + AGI: 245 + 25 = 270
    if (ResolveEVA(50, 250, 0, false) != 270)
    {
        return false;
    }

    // useModAsBase flags
    if (!UseModAsEVABase(true, false, false) || !UseModAsEVABase(false, true, false))
    {
        return false;
    }
    if (UseModAsEVABase(false, true, true) || UseModAsEVABase(false, false, false))
    {
        return false;
    }

    // Floor at 1
    if (ResolveEVA(0, 0, 0, false) != 1 || ResolveEVA(0, 0, 0, true) != 1)
    {
        return false;
    }
    if (ResolveDEF(0, 0.0f, false, 0, 0, -200, 0, 0, false) != 1)
    {
        return false;
    }

    return true;
}
} // namespace

auto runDefEva1638SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "def_eva_1638 self-tests failed\n";
        return false;
    }
    return true;
}
