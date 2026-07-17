#include "test_treasure_flush_plan_2781.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure flush plan 2781 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTreasureFlushPlan2781SelfTests() -> bool
{
    using treasurepoolhelpers::FlushPlan;
    using treasurepoolhelpers::PlanFlush;
    using treasurepoolhelpers::ShouldFlushPool;

    bool ok = true;

    // Empty pool: no checks.
    {
        const FlushPlan plan = PlanFlush(0);
        ok = expect(!plan.runChecks, "empty no checks") && ok;
    }

    // Non-empty pool: run checks for each slot.
    {
        const FlushPlan plan = PlanFlush(1);
        ok = expect(plan.runChecks, "one item runs checks") && ok;
    }
    {
        const FlushPlan plan = PlanFlush(10);
        ok = expect(plan.runChecks, "full pool runs checks") && ok;
    }
    {
        const FlushPlan plan = PlanFlush(255);
        ok = expect(plan.runChecks, "max count runs checks") && ok;
    }

    // PlanFlush must wrap ShouldFlushPool exactly.
    ok = expect(ShouldFlushPool(1) && !ShouldFlushPool(0), "helper flush gate") && ok;

    const uint8 composeCounts[] = { 0, 1, 2, 5, 10, 255 };
    for (const uint8 count : composeCounts)
    {
        const FlushPlan plan = PlanFlush(count);
        ok = expect(plan.runChecks == ShouldFlushPool(count), "compose match") && ok;
    }

    return ok;
}
