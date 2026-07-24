#include "test_action_queue_due_6332.h"

#include "map/ai/helpers/action_queue_capacity.h"

#include <chrono>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "action queue due 6332 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CAIActionQueue::checkAction due gate (slice 6332).
auto runActionQueueDue6332SelfTests() -> bool
{
    using actionqueuehelpers::ActionDueStrict;
    using actionqueuehelpers::IsBothQueuesEmpty;
    using Clock = std::chrono::steady_clock;

    bool ok = true;

    const auto due    = Clock::time_point{ std::chrono::seconds{ 5 } };
    const auto before = due - std::chrono::nanoseconds{ 1 };
    const auto at     = due;
    const auto after  = due + std::chrono::nanoseconds{ 1 };

    ok = expect(!ActionDueStrict(before, due), "before due") && ok;
    ok = expect(!ActionDueStrict(at, due), "equal due strict") && ok;
    ok = expect(ActionDueStrict(after, due), "after due") && ok;
    ok = expect(IsBothQueuesEmpty(true, true), "6331 residual") && ok;

    return ok;
}
