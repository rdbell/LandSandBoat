#include "test_universal_setitem_plan_2801.h"

#include "map/universal_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universal setitem plan 2801 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runUniversalSetItemPlan2801SelfTests() -> bool
{
    using namespace ucontainerhelpers;

    bool ok = true;

    // ShouldAllowSetItem: in-range && !locked
    ok = expect(ShouldAllowSetItem(true, false), "allow in-range unlocked") && ok;
    ok = expect(!ShouldAllowSetItem(true, true), "reject locked") && ok;
    ok = expect(!ShouldAllowSetItem(false, false), "reject out-of-range") && ok;
    ok = expect(!ShouldAllowSetItem(false, true), "reject out-of-range locked") && ok;

    // PlanSetItemCountDelta truth table
    ok = expect(PlanSetItemCountDelta(true, false) == 1, "set non-null into empty → +1") && ok;
    ok = expect(PlanSetItemCountDelta(false, true) == -1, "clear occupied → -1") && ok;
    ok = expect(PlanSetItemCountDelta(true, true) == 0, "replace occupied non-null → 0") && ok;
    ok = expect(PlanSetItemCountDelta(false, false) == 0, "null into empty → 0") && ok;

    return ok;
}
