#include "test_synth_result_7082.h"

#include "map/synth_result.h"

#include <iostream>

auto runSynthResult7082SelfTests() -> bool
{
    using synthresulthelpers::MakeSkillPlan;
    using synthresulthelpers::SkillPlan;

    const bool ok = MakeSkillPlan(0, 0, true) == SkillPlan{ .successRate = 95.0f, .hqTier = 1, .canHQ = true } &&
                    MakeSkillPlan(4, 0, true) == SkillPlan{ .successRate = 70.0f, .hqTier = 0, .canHQ = false } &&
                    MakeSkillPlan(1, 0, true) == SkillPlan{ .successRate = 90.0f, .hqTier = 0, .canHQ = false } &&
                    MakeSkillPlan(-11, 0, true) == SkillPlan{ .successRate = 95.0f, .hqTier = 2, .canHQ = true } &&
                    MakeSkillPlan(0, 10, false) == SkillPlan{ .successRate = 99.0f, .hqTier = 1, .canHQ = false };
    if (!ok)
    {
        std::cerr << "synth result 7082 self-test failed\n";
    }
    return ok;
}
