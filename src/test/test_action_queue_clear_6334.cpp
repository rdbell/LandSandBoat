#include "test_action_queue_clear_6334.h"

#include "map/ai/helpers/action_queue_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "action queue clear 6334 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for clearActionQueue / clearTimerQueue (slice 6334).
auto runActionQueueClear6334SelfTests() -> bool
{
    using actionqueuehelpers::ActionStateGateAllows;
    using actionqueuehelpers::ShouldContinueClear;
    using actionqueuehelpers::ShouldRouteToActionQueue;

    bool ok = true;

    ok = expect(!ShouldContinueClear(true) && ShouldContinueClear(false), "clear while identity") && ok;
    ok = expect(ShouldRouteToActionQueue(true) && !ShouldRouteToActionQueue(false), "6333 residual route") && ok;
    ok = expect(ActionStateGateAllows(true, true) && !ActionStateGateAllows(true, false), "6333 residual gate") && ok;

    return ok;
}
