#include "test_aicontainer_change_state_host_6368.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer ChangeState host 6368 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for ChangeState / ForceChangeState gate residuals (slice 6368).
// Go host halves aicontainer.ChangeState / ForceChangeState drive Stack under these gates.
auto runAicontainerChangeStateHost6368SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanPushState;
    using aicontainerhelpers::ShouldCleanupCompletedState;
    using aicontainerhelpers::ShouldSuspendCurrentOnEnter;
    using aicontainerhelpers::TickStateLoopContinue;

    bool ok = true;

    // CanPushState residual (6312) — ChangeState/ForceChangeState ceiling.
    ok = expect(CanPushState(0), "push count 0") && ok;
    ok = expect(CanPushState(10), "push count 10") && ok;
    ok = expect(!CanPushState(11), "push count 11") && ok;

    // CanChangeState residual (3303) — ChangeState only.
    ok = expect(CanChangeState(false, false), "idle can change") && ok;
    ok = expect(CanChangeState(true, true), "current allows") && ok;
    ok = expect(!CanChangeState(true, false), "current blocks") && ok;

    // CheckCompletedStates residual (6322).
    ok = expect(ShouldCleanupCompletedState(true, true), "completed cleanup") && ok;
    ok = expect(!ShouldCleanupCompletedState(true, false), "not completed") && ok;
    ok = expect(!ShouldCleanupCompletedState(false, true), "idle no cleanup") && ok;

    // Enter residual (6327 / 6366).
    ok = expect(ShouldSuspendCurrentOnEnter(true), "suspend has current") && ok;
    ok = expect(TickStateLoopContinue(32), "6367 residual") && ok;

    return ok;
}
