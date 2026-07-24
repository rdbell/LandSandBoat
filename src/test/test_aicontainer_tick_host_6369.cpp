#include "test_aicontainer_tick_host_6369.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Tick host 6369 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Tick orchestration phase-gate residuals (slice 6369).
// Go host half aicontainer.Tick composes ActionQueue → Pathing → Controller →
// StateDrain → PreventPark under these gates.
auto runAicontainerTickHost6369SelfTests() -> bool
{
    using aicontainerhelpers::CanPushState;
    using aicontainerhelpers::ShouldCheckActionQueue;
    using aicontainerhelpers::ShouldTickController;
    using aicontainerhelpers::ShouldTickFollowPath;
    using aicontainerhelpers::TickPreventActionParkAllowed;
    using aicontainerhelpers::TickStateLoopContinue;

    bool ok = true;

    // ActionQueue always present on production Tick (hasQueue=true).
    ok = expect(ShouldCheckActionQueue(true), "queue present") && ok;
    ok = expect(!ShouldCheckActionQueue(false), "queue absent") && ok;

    // Pathing only without controller.
    ok = expect(ShouldTickFollowPath(false, true, false), "path no ctrl") && ok;
    ok = expect(!ShouldTickFollowPath(true, true, false), "path blocked by ctrl") && ok;

    // Controller only when present and canUpdate.
    ok = expect(ShouldTickController(true, true), "ctrl tick") && ok;
    ok = expect(!ShouldTickController(true, false), "ctrl !canUpdate") && ok;
    ok = expect(!ShouldTickController(false, true), "no ctrl") && ok;

    // Drain + park residuals used inside orchestration.
    ok = expect(TickStateLoopContinue(32), "drain guard 32") && ok;
    ok = expect(TickPreventActionParkAllowed(true, true, false, false, false, true), "park admit") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, true, false, true, false, true), "park magic block") && ok;

    // ChangeState residual (6368) still holds.
    ok = expect(CanPushState(10), "6368 residual") && ok;

    return ok;
}
