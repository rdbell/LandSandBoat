#include "test_action_queue_is_empty_6331.h"

#include "map/ai/helpers/action_queue_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "action queue isEmpty 6331 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CAIActionQueue::isEmpty (slice 6331).
auto runActionQueueIsEmpty6331SelfTests() -> bool
{
    using actionqueuehelpers::IsBothQueuesEmpty;

    bool ok = true;

    ok = expect(IsBothQueuesEmpty(true, true), "both empty") && ok;
    ok = expect(!IsBothQueuesEmpty(true, false), "timer non-empty") && ok;
    ok = expect(!IsBothQueuesEmpty(false, true), "action non-empty") && ok;
    ok = expect(!IsBothQueuesEmpty(false, false), "neither empty") && ok;

    return ok;
}
