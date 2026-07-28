#include "test_desynth_result_7083.h"

#include "map/desynth_result.h"

#include <iostream>

auto runDesynthResult7083SelfTests() -> bool
{
    using desynthresulthelpers::MakeSkillPlan;
    using desynthresulthelpers::SkillPlan;

    const bool ok = MakeSkillPlan(0, 0, true) == SkillPlan{ .successRate = 40.0f, .canHQ = true } &&
                    MakeSkillPlan(1, 0, true) == SkillPlan{ .successRate = 40.0f, .canHQ = true } &&
                    MakeSkillPlan(8, 0, true) == SkillPlan{ .successRate = 10.0f - 10.0f / 3.0f, .canHQ = true } &&
                    MakeSkillPlan(10, 0, true) == SkillPlan{ .successRate = 0.0f, .canHQ = true } &&
                    MakeSkillPlan(0, 100, false) == SkillPlan{ .successRate = 141.0f, .canHQ = false };
    if (!ok)
    {
        std::cerr << "desynth result 7083 self-test failed\n";
    }
    return ok;
}
