#include "test_aicontainer_enter_resume_6327.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer enter/resume 6327 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for enterState / resumeNextState (slice 6327).
auto runAicontainerEnterResume6327SelfTests() -> bool
{
    using aicontainerhelpers::IsActionQueueEmpty;
    using aicontainerhelpers::ShouldCleanupCompletedState;
    using aicontainerhelpers::ShouldClearCurrentState;
    using aicontainerhelpers::ShouldResumeStackedState;
    using aicontainerhelpers::ShouldSuspendCurrentOnEnter;

    bool ok = true;

    ok = expect(ShouldSuspendCurrentOnEnter(true) && !ShouldSuspendCurrentOnEnter(false), "suspend identity") && ok;
    for (const bool has : { false, true })
    {
        ok = expect(ShouldSuspendCurrentOnEnter(has) == has, "suspend free==identity") && ok;
    }

    ok = expect(!ShouldResumeStackedState(true) && ShouldResumeStackedState(false), "resume identity") && ok;
    for (const bool empty : { false, true })
    {
        ok = expect(ShouldResumeStackedState(empty) == !empty, "resume free==!empty") && ok;
    }

    ok = expect(ShouldClearCurrentState(true) && !ShouldClearCurrentState(false), "6322 residual clear") && ok;
    ok = expect(ShouldCleanupCompletedState(true, true) && !ShouldCleanupCompletedState(true, false), "6322 residual cleanup") && ok;
    ok = expect(IsActionQueueEmpty(true) && !IsActionQueueEmpty(false), "6323 residual queue") && ok;

    return ok;
}
