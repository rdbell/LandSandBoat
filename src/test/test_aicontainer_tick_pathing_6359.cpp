#include "test_aicontainer_tick_pathing_6359.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer TickPathing 6359 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for AI Tick pathing admission (slice 6359).
// Go host half drives pathfind.FollowPath via aicontainer.TickPathing.
auto runAicontainerTickPathing6359SelfTests() -> bool
{
    using aicontainerhelpers::CanFollowPath;
    using aicontainerhelpers::ShouldNotifyPathPoint;
    using aicontainerhelpers::ShouldTickFollowPath;

    bool ok = true;

    ok = expect(!ShouldTickFollowPath(true, true, false), "controller → false") && ok;
    ok = expect(!ShouldTickFollowPath(false, false, false), "!canFollow → false") && ok;
    ok = expect(!ShouldTickFollowPath(false, true, true), "paused → false") && ok;
    ok = expect(ShouldTickFollowPath(false, true, false), "admit → true") && ok;
    ok = expect(ShouldNotifyPathPoint(true) && !ShouldNotifyPathPoint(false), "notify") && ok;
    // Compose with CanFollowPath residual (6306).
    ok = expect(ShouldTickFollowPath(false, CanFollowPath(true, false, false), false), "compose canFollow idle") && ok;
    ok = expect(!ShouldTickFollowPath(false, CanFollowPath(true, true, false), false), "compose canFollow blocked") && ok;
    ok = expect(CanFollowPath(true, false, false), "6306 residual") && ok;

    return ok;
}
