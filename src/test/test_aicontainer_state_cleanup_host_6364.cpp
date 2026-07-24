#include "test_aicontainer_state_cleanup_host_6364.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer state cleanup host 6364 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for ClearStateStack / InterruptStates / CheckCompletedStates
// loop admissions (slice 6364). Go host halves drive cleanup inject loops.
auto runAicontainerStateCleanupHost6364SelfTests() -> bool
{
    using aicontainerhelpers::DespawnShouldDispatchController;
    using aicontainerhelpers::ShouldCleanupCompletedState;
    using aicontainerhelpers::ShouldClearCurrentState;
    using aicontainerhelpers::ShouldInterruptCurrent;

    bool ok = true;

    ok = expect(ShouldClearCurrentState(true), "clear has current") && ok;
    ok = expect(!ShouldClearCurrentState(false), "clear empty") && ok;
    ok = expect(ShouldInterruptCurrent(true, true), "interrupt admit") && ok;
    ok = expect(!ShouldInterruptCurrent(true, false), "interrupt !can") && ok;
    ok = expect(!ShouldInterruptCurrent(false, true), "interrupt no current") && ok;
    ok = expect(ShouldCleanupCompletedState(true, true), "completed admit") && ok;
    ok = expect(!ShouldCleanupCompletedState(true, false), "completed incomplete") && ok;
    ok = expect(!ShouldCleanupCompletedState(false, true), "completed no current") && ok;
    ok = expect(DespawnShouldDispatchController(true), "6363 residual") && ok;

    return ok;
}
