#include "test_fishing_skillup_1630.h"

#include "map/fishing_skillup_capacity.h"

#include <iostream>

namespace
{
using namespace fishingskilluphelpers;

auto Check() -> bool
{
    {
        FishingSkillupParams p{};
        p.successType    = FishingSuccessNone;
        p.catchLevel     = 30;
        p.charSkillLevel = 20;
        p.multiplier     = 1.0f;
        if (PlanFishingSkillup(p).eligible)
        {
            return false;
        }
    }
    {
        FishingSkillupParams p{};
        p.successType    = 2; // small
        p.catchLevel     = 20;
        p.charSkillLevel = 20;
        p.multiplier     = 1.0f;
        if (PlanFishingSkillup(p).eligible)
        {
            return false;
        }
    }
    {
        FishingSkillupParams p{};
        p.successType    = 2;
        p.catchLevel     = 30;
        p.charSkillLevel = 20;
        p.charSkill      = 200;
        p.skillRank      = 2;
        p.multiplier     = 1.0f;
        p.inCity         = true;
        p.moonPhase      = 50;
        p.moonDirection  = 0;
        const auto plan  = PlanFishingSkillup(p);
        if (!plan.eligible || plan.maxSkill != 300 || plan.maxSkillAmount != 3 || plan.skillRoll != 76)
        {
            return false;
        }
        if (plan.maxChance < 4)
        {
            return false;
        }
    }
    {
        FishingSkillupParams p{};
        p.successType    = 2;
        p.catchLevel     = 30;
        p.charSkillLevel = 20;
        p.skillRank      = 0;
        p.multiplier     = 1.0f;
        p.inCity         = true;
        p.moonPhase      = 50;
        p.hasLuShangRod  = true;
        const auto with  = PlanFishingSkillup(p);
        p.hasLuShangRod  = false;
        const auto without = PlanFishingSkillup(p);
        if (with.skillRoll != without.skillRoll + 20)
        {
            return false;
        }
    }
    if (ClampSkillAmount(5, 298, 300) != 2 || ClampSkillAmount(1, 100, 300) != 1)
    {
        return false;
    }
    if (!SkillLevelCrossed(99, 2) || SkillLevelCrossed(90, 5))
    {
        return false;
    }
    return true;
}
} // namespace

auto runFishingSkillup1630SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fishing_skillup_1630 self-tests failed\n";
        return false;
    }
    return true;
}
