#include "test_aicontainer_action_queue_park_host_6365.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer ActionQueue/park host 6365 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for ActionQueue surface + prevent-action park residual (6365).
// Go host halves drive actionqueue.Executor and parkInactive inject.
auto runAicontainerActionQueueParkHost6365SelfTests() -> bool
{
    using aicontainerhelpers::IsActionQueueEmpty;
    using aicontainerhelpers::ShouldCheckActionQueue;
    using aicontainerhelpers::ShouldClearCurrentState;
    using aicontainerhelpers::TickPreventActionParkAllowed;

    bool ok = true;

    ok = expect(IsActionQueueEmpty(true), "empty true") && ok;
    ok = expect(!IsActionQueueEmpty(false), "empty false") && ok;
    ok = expect(ShouldCheckActionQueue(true), "6362 residual") && ok;
    ok = expect(TickPreventActionParkAllowed(true, true, false, false, false, true), "park admit") && ok;
    ok = expect(!TickPreventActionParkAllowed(true, true, false, true, false, true), "park magic block") && ok;
    ok = expect(ShouldClearCurrentState(true), "6364 residual") && ok;

    return ok;
}
