#include "test_pathfind_look_wait_6342.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <chrono>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind look/wait 6342 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for LookAt + FollowPath wait/complete (slice 6342).
auto runPathfindLookWait6342SelfTests() -> bool
{
    using pathfindstatushelpers::PathIndexComplete;
    using pathfindstatushelpers::ShouldResumePatrol;
    using pathfindstatushelpers::ShouldUpdateLookAt;
    using pathfindstatushelpers::WaitStillActive;
    using Clock = std::chrono::steady_clock;

    bool ok = true;

    ok = expect(ShouldUpdateLookAt(false) && !ShouldUpdateLookAt(true), "look admission") && ok;

    const auto t0 = Clock::time_point{ std::chrono::seconds{ 5 } };
    const auto t1 = Clock::time_point{ std::chrono::seconds{ 10 } };
    ok = expect(WaitStillActive(t0, t1), "still waiting") && ok;
    ok = expect(!WaitStillActive(t1, t1), "at deadline advances") && ok;
    ok = expect(!WaitStillActive(t1 + std::chrono::seconds{ 1 }, t1), "after deadline advances") && ok;

    ok = expect(!PathIndexComplete(0, 3) && PathIndexComplete(3, 3) && PathIndexComplete(4, 3), "path complete") && ok;
    ok = expect(ShouldResumePatrol(true), "6341 residual") && ok;

    return ok;
}
