#include "test_aicontainer_enter_resume_host_6366.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Enter/Resume host 6366 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for enterState / resumeNextState admissions (slice 6366).
// Go host halves drive Stack.PushCurrent / ResumeNext under these gates.
auto runAicontainerEnterResumeHost6366SelfTests() -> bool
{
    using aicontainerhelpers::IsActionQueueEmpty;
    using aicontainerhelpers::ShouldResumeStackedState;
    using aicontainerhelpers::ShouldSuspendCurrentOnEnter;
    using aicontainerhelpers::TickPreventActionParkAllowed;

    bool ok = true;

    ok = expect(ShouldSuspendCurrentOnEnter(true), "suspend has current") && ok;
    ok = expect(!ShouldSuspendCurrentOnEnter(false), "suspend idle") && ok;
    ok = expect(ShouldResumeStackedState(false), "resume non-empty stack") && ok;
    ok = expect(!ShouldResumeStackedState(true), "resume empty → idle") && ok;
    ok = expect(IsActionQueueEmpty(true), "6365 residual") && ok;
    ok = expect(TickPreventActionParkAllowed(true, true, false, false, false, true), "6365 park residual") && ok;

    return ok;
}
