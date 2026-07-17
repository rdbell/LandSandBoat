#include "test_merit_raise_plan_2805.h"

#include "map/merit_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "merit raise plan 2805 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU16(const uint16 got, const uint16 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "merit raise plan 2805 self-test failed: " << label
                  << " got=" << got << " want=" << want << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMeritRaisePlan2805SelfTests() -> bool
{
    using namespace meritshelpers;

    bool ok = true;

    // --- ShouldRaiseMerit: success ---
    ok = expect(ShouldRaiseMerit(1, 1, 0, 15, 0, 75), "exact points") && ok;
    ok = expect(ShouldRaiseMerit(20, 1, 0, 15, 0, 75), "surplus points") && ok;
    ok = expect(ShouldRaiseMerit(5, 5, 4, 15, 9, 10), "exact under category cap") && ok;

    // --- ShouldRaiseMerit: insufficient points ---
    ok = expect(!ShouldRaiseMerit(0, 1, 0, 15, 0, 75), "zero points") && ok;
    ok = expect(!ShouldRaiseMerit(4, 5, 4, 15, 0, 75), "short one point") && ok;

    // --- ShouldRaiseMerit: upgrade max ---
    ok = expect(!ShouldRaiseMerit(100, 1, 5, 5, 0, 10), "count == upgradeMax") && ok;
    ok = expect(!ShouldRaiseMerit(100, 1, 6, 5, 0, 10), "count > upgradeMax") && ok;
    ok = expect(ShouldRaiseMerit(100, 1, 4, 5, 0, 10), "count just under upgradeMax") && ok;

    // --- ShouldRaiseMerit: category cap ---
    ok = expect(!ShouldRaiseMerit(100, 1, 0, 5, 10, 10), "categoryCount == MaxPoints") && ok;
    ok = expect(!ShouldRaiseMerit(100, 1, 0, 5, 11, 10), "categoryCount > MaxPoints") && ok;
    ok = expect(ShouldRaiseMerit(100, 1, 0, 5, 9, 10), "categoryCount just under MaxPoints") && ok;

    // --- PlanRaiseMerit: blocked (spend=0) ---
    {
        const auto plan = PlanRaiseMerit(0, 1, 0, 15, 0, 75);
        ok = expect(!plan.apply && plan.spend == 0, "plan blocked zero points") && ok;
    }
    {
        const auto plan = PlanRaiseMerit(50, 1, 5, 5, 0, 10);
        ok = expect(!plan.apply && plan.spend == 0, "plan blocked upgrade max") && ok;
    }
    {
        const auto plan = PlanRaiseMerit(50, 3, 0, 5, 10, 10);
        ok = expect(!plan.apply && plan.spend == 0, "plan blocked category cap") && ok;
    }

    // --- PlanRaiseMerit: apply (spend=nextCost) ---
    {
        const auto plan = PlanRaiseMerit(1, 1, 0, 15, 0, 75);
        ok = expect(plan.apply, "plan apply exact") && ok;
        ok = expectU16(plan.spend, 1, "plan spend exact") && ok;
    }
    {
        const auto plan = PlanRaiseMerit(20, 5, 4, 15, 4, 75);
        ok = expect(plan.apply, "plan apply cost 5") && ok;
        ok = expectU16(plan.spend, 5, "plan spend cost 5") && ok;
    }
    {
        // Job group 1 style: cost 1, category almost full.
        const auto plan = PlanRaiseMerit(50, 1, 0, 5, 9, 10);
        ok = expect(plan.apply, "plan apply under category cap") && ok;
        ok = expectU16(plan.spend, 1, "plan spend under category cap") && ok;
    }

    return ok;
}
