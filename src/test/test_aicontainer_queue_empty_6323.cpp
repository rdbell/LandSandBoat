#include "test_aicontainer_queue_empty_6323.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer QueueEmpty 6323 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for QueueEmpty (slice 6323).
auto runAicontainerQueueEmpty6323SelfTests() -> bool
{
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::IsActionQueueEmpty;
    using aicontainerhelpers::ShouldClearCurrentState;
    using aicontainerhelpers::ShouldCleanupCompletedState;

    bool ok = true;

    ok = expect(IsActionQueueEmpty(true) && !IsActionQueueEmpty(false), "identity inject") && ok;
    for (const bool empty : { false, true })
    {
        ok = expect(IsActionQueueEmpty(empty) == empty, "free==identity") && ok;
    }

    ok = expect(!CanDispatch(false) && CanDispatch(true), "CanDispatch residual") && ok;
    ok = expect(ShouldClearCurrentState(true) && !ShouldClearCurrentState(false), "6322 residual clear") && ok;
    ok = expect(ShouldCleanupCompletedState(true, true) && !ShouldCleanupCompletedState(true, false),
                "6322 residual cleanup") &&
         ok;

    return ok;
}
