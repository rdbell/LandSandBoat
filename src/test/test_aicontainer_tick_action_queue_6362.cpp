#include "test_aicontainer_tick_action_queue_6362.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer TickActionQueue 6362 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for AI Tick ActionQueue.checkAction admission (slice 6362).
// Go host half drives actionqueue.Executor.Check via aicontainer.TickActionQueue.
auto runAicontainerTickActionQueue6362SelfTests() -> bool
{
    using aicontainerhelpers::ShouldCheckActionQueue;
    using aicontainerhelpers::ShouldTickController;
    using aicontainerhelpers::ShouldTickFollowPath;

    bool ok = true;

    ok = expect(!ShouldCheckActionQueue(false), "no queue") && ok;
    ok = expect(ShouldCheckActionQueue(true), "has queue") && ok;
    // Production Tick always has ActionQueue member.
    ok = expect(ShouldCheckActionQueue(true), "production member") && ok;
    ok = expect(ShouldTickController(true, true), "6361 residual") && ok;
    ok = expect(ShouldTickFollowPath(false, true, false), "6359 residual") && ok;

    return ok;
}
