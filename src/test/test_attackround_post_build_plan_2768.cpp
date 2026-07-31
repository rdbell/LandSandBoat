#include "test_attackround_post_build_plan_2768.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround post build plan 2768 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAttackRoundPostBuildPlan2768SelfTests() -> bool
{
    using namespace attackroundhelpers;

    const auto empty   = PlanAttackRoundPostBuild(0);
    const auto one     = PlanAttackRoundPostBuild(1);
    const auto several = PlanAttackRoundPostBuild(3);
    const auto maxed   = PlanAttackRoundPostBuild(8);
    const auto negative = PlanAttackRoundPostBuild(-1);

    return expect(!empty.setFirstSwing && empty.deleteHasteSamba, "empty swings") &&
           expect(one.setFirstSwing && one.deleteHasteSamba, "one swing") &&
           expect(several.setFirstSwing && several.deleteHasteSamba, "several swings") &&
           expect(maxed.setFirstSwing && maxed.deleteHasteSamba, "max swings") &&
           expect(negative.setFirstSwing && negative.deleteHasteSamba, "negative synthetic count wraps");
}
