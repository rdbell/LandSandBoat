#include "test_ability_recast_plan_6853.h"

#include "map/misc_progress_capacity.h"

#include <cstddef>
#include <initializer_list>
#include <iostream>

namespace
{
auto Matches(const miscprogresshelpers::AbilityRecastPlan& plan, std::initializer_list<miscprogresshelpers::AbilityRecastStep> want) -> bool
{
    if (plan.count != want.size())
    {
        return false;
    }

    std::size_t index = 0;
    for (const auto& expected : want)
    {
        const auto& actual = plan.steps[index++];
        if (actual.recastId != expected.recastId || actual.usesCharges != expected.usesCharges)
        {
            return false;
        }
    }
    return true;
}

auto Check() -> bool
{
    using miscprogresshelpers::AbilityRecastStep;
    using miscprogresshelpers::PlanAbilityRecast;
    using miscprogresshelpers::RecastBloodPactRage;
    using miscprogresshelpers::RecastBloodPactWard;
    using miscprogresshelpers::RecastInnin;
    using miscprogresshelpers::RecastYonin;

    return Matches(PlanAbilityRecast(10, false, false), { { 10, false } }) &&
           Matches(PlanAbilityRecast(10, true, false), { { 10, true } }) &&
           Matches(PlanAbilityRecast(RecastBloodPactRage, true, true), { { RecastBloodPactRage, true }, { RecastBloodPactWard, false } }) &&
           Matches(PlanAbilityRecast(RecastBloodPactWard, false, true), { { RecastBloodPactWard, false }, { RecastBloodPactRage, false } }) &&
           Matches(PlanAbilityRecast(RecastBloodPactRage, false, false), { { RecastBloodPactRage, false } }) &&
           Matches(PlanAbilityRecast(RecastYonin, false, false), { { RecastYonin, false }, { RecastInnin, false } });
}
} // namespace

auto runAbilityRecastPlan6853SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "ability recast plan 6853 self-test failed\\n";
    }
    return ok;
}
