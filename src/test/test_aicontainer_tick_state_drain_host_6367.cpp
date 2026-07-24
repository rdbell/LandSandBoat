#include "test_aicontainer_tick_state_drain_host_6367.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer TickStateDrain host 6367 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Tick state-drain guard residual (slice 6367).
// Go host half aicontainer.TickStateDrain drives DoUpdate/cleanup injects.
auto runAicontainerTickStateDrainHost6367SelfTests() -> bool
{
    using aicontainerhelpers::ShouldResumeStackedState;
    using aicontainerhelpers::ShouldSuspendCurrentOnEnter;
    using aicontainerhelpers::TickStateLoopContinue;

    bool ok = true;

    ok = expect(TickStateLoopContinue(1), "guard 1") && ok;
    ok = expect(TickStateLoopContinue(32), "guard 32") && ok;
    ok = expect(!TickStateLoopContinue(33), "guard 33") && ok;
    ok = expect(ShouldSuspendCurrentOnEnter(true), "6366 residual") && ok;
    ok = expect(ShouldResumeStackedState(false), "6366 resume residual") && ok;

    return ok;
}
