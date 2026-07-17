#include "test_treasure_post_lot_plan_2779.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure post lot plan 2779 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTreasurePostLotPlan2779SelfTests() -> bool
{
    using treasurepoolhelpers::HigherLotSelection;
    using treasurepoolhelpers::IsHigherLot;
    using treasurepoolhelpers::PlanPostLot;
    using treasurepoolhelpers::PostLotPlan;
    using treasurepoolhelpers::ShouldEvaluateTreasureImmediately;

    bool ok = true;

    // IsHigherLot: strict greater-than (ties keep prior highest).
    ok = expect(IsHigherLot(50, 10), "higher 50>10") && ok;
    ok = expect(!IsHigherLot(10, 50), "not higher 10<50") && ok;
    ok = expect(!IsHigherLot(10, 10), "tie not higher") && ok;
    ok = expect(IsHigherLot(1, 0), "higher from zero") && ok;
    ok = expect(!IsHigherLot(0, 0), "zero tie") && ok;

    // HigherLotSelection: candidate wins only when strictly higher.
    ok = expect(HigherLotSelection(10, 50) == 50, "select higher") && ok;
    ok = expect(HigherLotSelection(50, 10) == 50, "keep higher") && ok;
    ok = expect(HigherLotSelection(10, 10) == 10, "tie keeps current") && ok;
    ok = expect(HigherLotSelection(0, 1) == 1, "select from zero") && ok;

    // Simulate highest-lot scan with HigherLotSelection + IsHigherLot.
    {
        uint16 highest = 0;
        const uint16 lots[] = { 10, 50, 40, 50 };
        for (const uint16 lot : lots)
        {
            if (IsHigherLot(lot, highest))
            {
                highest = HigherLotSelection(highest, lot);
            }
        }
        ok = expect(highest == 50, "scan first-wins max 50") && ok;
    }
    {
        uint16 highest = 0;
        const uint16 lots[] = { 1, 2, 3 };
        for (const uint16 lot : lots)
        {
            highest = HigherLotSelection(highest, lot);
        }
        ok = expect(highest == 3, "scan ascending to 3") && ok;
    }

    // ShouldEvaluateTreasureImmediately: all members have lotted.
    ok = expect(ShouldEvaluateTreasureImmediately(1, 1), "solo all lotted") && ok;
    ok = expect(ShouldEvaluateTreasureImmediately(3, 3), "party all lotted") && ok;
    ok = expect(!ShouldEvaluateTreasureImmediately(2, 3), "partial lotters") && ok;
    ok = expect(!ShouldEvaluateTreasureImmediately(0, 1), "none lotted") && ok;
    ok = expect(ShouldEvaluateTreasureImmediately(0, 0), "empty pool equal") && ok;
    ok = expect(!ShouldEvaluateTreasureImmediately(4, 3), "more lotters than members") && ok;

    // PlanPostLot: evaluateImmediately mirrors ShouldEvaluateTreasureImmediately.
    {
        const PostLotPlan plan = PlanPostLot(1, 1);
        ok = expect(plan.evaluateImmediately, "plan solo immediate") && ok;
    }
    {
        const PostLotPlan plan = PlanPostLot(2, 3);
        ok = expect(!plan.evaluateImmediately, "plan partial defer") && ok;
    }
    {
        const PostLotPlan plan = PlanPostLot(6, 6);
        ok = expect(plan.evaluateImmediately, "plan alliance full") && ok;
    }
    {
        const PostLotPlan plan = PlanPostLot(0, 0);
        ok = expect(plan.evaluateImmediately, "plan empty equal") && ok;
    }

    // Compose helpers: PlanPostLot flag must match ShouldEvaluateTreasureImmediately.
    const struct
    {
        std::size_t lotterCount;
        std::size_t memberCount;
        const char* label;
    } composeCases[] = {
        { 1, 1, "compose solo" },
        { 2, 3, "compose partial" },
        { 3, 3, "compose full" },
        { 0, 5, "compose none" },
        { 5, 4, "compose overshoot" },
    };
    for (const auto& c : composeCases)
    {
        const PostLotPlan plan = PlanPostLot(c.lotterCount, c.memberCount);
        const bool        want = ShouldEvaluateTreasureImmediately(c.lotterCount, c.memberCount);
        ok = expect(plan.evaluateImmediately == want, c.label) && ok;
    }

    return ok;
}
