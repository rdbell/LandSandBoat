#include "test_merit_lower_plan_2810.h"

#include "map/merit_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "merit lower plan 2810 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMeritLowerPlan2810SelfTests() -> bool
{
    using namespace meritshelpers;

    bool ok = true;

    // --- ShouldLowerMerit: success ---
    ok = expect(ShouldLowerMerit(1), "count 1") && ok;
    ok = expect(ShouldLowerMerit(2), "count 2") && ok;
    ok = expect(ShouldLowerMerit(15), "count 15") && ok;
    ok = expect(ShouldLowerMerit(255), "count max uint8") && ok;

    // --- ShouldLowerMerit: blocked ---
    ok = expect(!ShouldLowerMerit(0), "count 0") && ok;

    // --- PlanLowerMerit: blocked ---
    {
        const auto plan = PlanLowerMerit(false, 0);
        ok = expect(!plan.apply, "absent merit count 0") && ok;
    }
    {
        const auto plan = PlanLowerMerit(false, 5);
        ok = expect(!plan.apply, "absent merit with count") && ok;
    }
    {
        const auto plan = PlanLowerMerit(true, 0);
        ok = expect(!plan.apply, "present merit count 0") && ok;
    }

    // --- PlanLowerMerit: apply ---
    {
        const auto plan = PlanLowerMerit(true, 1);
        ok = expect(plan.apply, "present count 1") && ok;
    }
    {
        const auto plan = PlanLowerMerit(true, 5);
        ok = expect(plan.apply, "present count 5") && ok;
    }
    {
        const auto plan = PlanLowerMerit(true, 255);
        ok = expect(plan.apply, "present count max") && ok;
    }

    return ok;
}
