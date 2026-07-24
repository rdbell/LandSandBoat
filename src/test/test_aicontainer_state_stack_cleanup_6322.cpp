#include "test_aicontainer_state_stack_cleanup_6322.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer state stack cleanup 6322 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for ClearStateStack / CheckCompletedStates (slice 6322).
auto runAicontainerStateStackCleanup6322SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::ShouldClearCurrentState;
    using aicontainerhelpers::ShouldCleanupCompletedState;
    using aicontainerhelpers::ShouldInterruptCurrent;

    bool ok = true;

    ok = expect(ShouldClearCurrentState(true) && !ShouldClearCurrentState(false),
                "clear: presence identity") &&
         ok;

    ok = expect(ShouldCleanupCompletedState(true, true), "cleanup: completed current") && ok;
    ok = expect(!ShouldCleanupCompletedState(true, false), "cleanup: incomplete current") && ok;
    ok = expect(!ShouldCleanupCompletedState(false, true), "cleanup: no current") && ok;

    for (const bool has : { false, true })
    {
        ok = expect(ShouldClearCurrentState(has) == has, "clear free==identity") && ok;
        for (const bool completed : { false, true })
        {
            const bool want = has && completed;
            ok              = expect(ShouldCleanupCompletedState(has, completed) == want, "cleanup free==want") && ok;
        }
    }

    // Residual: InterruptStates still requires canInterrupt.
    ok = expect(ShouldInterruptCurrent(true, true) && !ShouldInterruptCurrent(true, false),
                "6321 residual: interrupt admission still holds") &&
         ok;
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6322") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6322") &&
         ok;

    return ok;
}
