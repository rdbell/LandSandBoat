#include "test_jobpoints_raise_plan_2803.h"

#include "map/job_points_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "jobpoints raise plan 2803 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU8(const uint8 got, const uint8 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "jobpoints raise plan 2803 self-test failed: " << label
                  << " got=" << static_cast<unsigned>(got)
                  << " want=" << static_cast<unsigned>(want) << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runJobPointsRaisePlan2803SelfTests() -> bool
{
    using namespace jobpointshelpers;

    bool ok = true;

    // --- JobPointCost pure form ---
    ok = expectU8(JobPointCost(0), 1, "cost(0)") && ok;
    ok = expectU8(JobPointCost(1), 2, "cost(1)") && ok;
    ok = expectU8(JobPointCost(19), 20, "cost(19)") && ok;
    ok = expectU8(JobPointCost(20), 0, "cost(20) blocks raise") && ok;
    ok = expectU8(JobPointCost(21), 1, "cost(21) wraps") && ok;

    // --- Missing job / type ---
    {
        const auto plan = PlanRaiseJobPoint(false, true, 0, 100);
        ok = expect(!plan.apply && plan.cost == 0, "missing job") && ok;
    }
    {
        const auto plan = PlanRaiseJobPoint(true, false, 0, 100);
        ok = expect(!plan.apply && plan.cost == 0, "missing jobPoint") && ok;
    }
    {
        const auto plan = PlanRaiseJobPoint(false, false, 5, 100);
        ok = expect(!plan.apply && plan.cost == 0, "missing both") && ok;
    }

    // --- Cost 0 at value 20 (cap) ---
    {
        const auto plan = PlanRaiseJobPoint(true, true, 20, 500);
        ok = expect(!plan.apply && plan.cost == 0, "value 20 cost 0 blocks") && ok;
    }

    // --- Insufficient points ---
    {
        const auto plan = PlanRaiseJobPoint(true, true, 9, 5); // cost 10
        ok = expect(!plan.apply && plan.cost == 10, "insufficient JP") && ok;
    }
    {
        const auto plan = PlanRaiseJobPoint(true, true, 0, 0); // cost 1
        ok = expect(!plan.apply && plan.cost == 1, "zero JP cost 1") && ok;
    }

    // --- Success cases ---
    {
        const auto plan = PlanRaiseJobPoint(true, true, 0, 1); // cost 1
        ok = expect(plan.apply && plan.cost == 1, "value 0 cost 1 exact") && ok;
    }
    {
        const auto plan = PlanRaiseJobPoint(true, true, 0, 50);
        ok = expect(plan.apply && plan.cost == 1, "value 0 cost 1 with surplus") && ok;
    }
    {
        const auto plan = PlanRaiseJobPoint(true, true, 19, 20); // cost 20
        ok = expect(plan.apply && plan.cost == 20, "value 19 cost 20 exact") && ok;
    }
    {
        const auto plan = PlanRaiseJobPoint(true, true, 19, 19); // cost 20, short by 1
        ok = expect(!plan.apply && plan.cost == 20, "value 19 short one") && ok;
    }
    {
        // Boundary: cost == currentJp
        const auto plan = PlanRaiseJobPoint(true, true, 4, 5); // cost 5
        ok = expect(plan.apply && plan.cost == 5, "exact equal JP") && ok;
    }

    // --- ShouldApplyRaiseJobPoint half ---
    ok = expect(ShouldApplyRaiseJobPoint(1, 1), "should apply cost 1 / jp 1") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(0, 100), "should not apply cost 0") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(10, 9), "should not apply short") && ok;
    ok = expect(ShouldApplyRaiseJobPoint(10, 10), "should apply exact") && ok;

    return ok;
}
