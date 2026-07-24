#include "test_action_queue_push_state_6333.h"

#include "map/ai/helpers/action_queue_capacity.h"

#include <chrono>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "action queue push/state 6333 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for pushAction routing + checkAction state gate (6333).
auto runActionQueuePushState6333SelfTests() -> bool
{
    using actionqueuehelpers::ActionDueStrict;
    using actionqueuehelpers::ActionStateGateAllows;
    using actionqueuehelpers::IsBothQueuesEmpty;
    using actionqueuehelpers::ShouldRouteToActionQueue;
    using Clock = std::chrono::steady_clock;

    bool ok = true;

    ok = expect(ShouldRouteToActionQueue(true) && !ShouldRouteToActionQueue(false), "route identity") && ok;
    ok = expect(ActionStateGateAllows(false, false) && ActionStateGateAllows(false, true), "no checkState always allows") && ok;
    ok = expect(!ActionStateGateAllows(true, false) && ActionStateGateAllows(true, true), "checkState needs canChange") && ok;

    const auto due   = Clock::time_point{ std::chrono::seconds{ 1 } };
    const auto after = due + std::chrono::nanoseconds{ 1 };
    ok = expect(ActionDueStrict(after, due), "6332 residual due") && ok;
    ok = expect(IsBothQueuesEmpty(true, true), "6331 residual empty") && ok;

    return ok;
}
