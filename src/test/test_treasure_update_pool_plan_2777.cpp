#include "test_treasure_update_pool_plan_2777.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure update pool plan 2777 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTreasureUpdatePoolPlan2777SelfTests() -> bool
{
    using treasurepoolhelpers::PlanUpdatePool;
    using treasurepoolhelpers::ShouldRejectNullMember;
    using treasurepoolhelpers::ShouldUpdatePoolForChar;
    using treasurepoolhelpers::UpdatePoolPlan;

    bool ok = true;

    // Happy path: member ok, not disappear → push trophy lists.
    {
        const UpdatePoolPlan plan = PlanUpdatePool(false, false, false);
        ok = expect(!plan.reject && plan.pushTrophyLists, "push visible") && ok;
    }

    // Member ok but DISAPPEAR → no push, no reject.
    {
        const UpdatePoolPlan plan = PlanUpdatePool(false, false, true);
        ok = expect(!plan.reject && !plan.pushTrophyLists, "disappear silent") && ok;
    }

    // Null char → reject, never push.
    {
        const UpdatePoolPlan plan = PlanUpdatePool(true, false, false);
        ok = expect(plan.reject && !plan.pushTrophyLists, "null char") && ok;
    }
    {
        // Even if isDisappear would otherwise suppress, reject wins.
        const UpdatePoolPlan plan = PlanUpdatePool(true, false, true);
        ok = expect(plan.reject && !plan.pushTrophyLists, "null char disappear") && ok;
    }

    // Pool mismatch → reject, never push.
    {
        const UpdatePoolPlan plan = PlanUpdatePool(false, true, false);
        ok = expect(plan.reject && !plan.pushTrophyLists, "pool mismatch") && ok;
    }
    {
        const UpdatePoolPlan plan = PlanUpdatePool(false, true, true);
        ok = expect(plan.reject && !plan.pushTrophyLists, "mismatch disappear") && ok;
    }

    // Both null and mismatch → still reject (short-circuit member).
    {
        const UpdatePoolPlan plan = PlanUpdatePool(true, true, false);
        ok = expect(plan.reject && !plan.pushTrophyLists, "null and mismatch") && ok;
    }

    // Compose existing helpers: plan flags must match Should* results.
    ok = expect(ShouldRejectNullMember(true, false), "helper reject null") && ok;
    ok = expect(ShouldRejectNullMember(false, true), "helper reject mismatch") && ok;
    ok = expect(!ShouldRejectNullMember(false, false), "helper accept member") && ok;
    ok = expect(ShouldUpdatePoolForChar(false) && !ShouldUpdatePoolForChar(true), "helper update char") && ok;

    const struct
    {
        bool charNull;
        bool poolMismatch;
        bool isDisappear;
        const char* label;
    } composeCases[] = {
        { false, false, false, "compose visible" },
        { false, false, true, "compose disappear" },
        { true, false, false, "compose null" },
        { false, true, true, "compose mismatch disappear" },
    };
    for (const auto& c : composeCases)
    {
        const UpdatePoolPlan plan       = PlanUpdatePool(c.charNull, c.poolMismatch, c.isDisappear);
        const bool           wantReject = ShouldRejectNullMember(c.charNull, c.poolMismatch);
        const bool           wantPush   = !wantReject && ShouldUpdatePoolForChar(c.isDisappear);
        ok = expect(plan.reject == wantReject && plan.pushTrophyLists == wantPush, c.label) && ok;
    }

    return ok;
}
