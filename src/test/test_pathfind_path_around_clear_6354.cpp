#include "test_pathfind_path_around_clear_6354.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind path around clear 6354 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for PathAround original-point and Clear wait (slice 6354).
auto runPathfindPathAroundClear6354SelfTests() -> bool
{
    using pathfindstatushelpers::ClearedWaitDeadline;
    using pathfindstatushelpers::ClearedWaiting;
    using pathfindstatushelpers::DistanceFromPointValue;
    using pathfindstatushelpers::OriginalPointX;
    using pathfindstatushelpers::OriginalPointY;
    using pathfindstatushelpers::OriginalPointZ;
    using pathfindstatushelpers::ResumePatrolCursor;
    using pathfindstatushelpers::ShouldNotifyZoneOnMove;
    using pathfindstatushelpers::ShouldUpdateClosestPatrol;

    bool ok = true;

    ok = expect(OriginalPointX(5.f) == 5.f, "orig X") && ok;
    ok = expect(OriginalPointY(2.f) == 2.f, "orig Y") && ok;
    ok = expect(OriginalPointZ(3.f) == 3.f, "orig Z") && ok;
    ok = expect(OriginalPointX(-1.5f) == -1.5f, "orig X neg") && ok;
    ok = expect(!ClearedWaiting(), "cleared waiting false") && ok;
    ok = expect(ClearedWaitDeadline() == 0, "cleared deadline 0") && ok;
    ok = expect(DistanceFromPointValue(2.f) == 2.f, "6347 residual range") && ok;
    ok = expect(ShouldNotifyZoneOnMove(true) && !ShouldNotifyZoneOnMove(false), "zone notify residual") && ok;
    ok = expect(ResumePatrolCursor() == 0 && ShouldUpdateClosestPatrol(1.f, 2.f), "6353 residual") && ok;

    return ok;
}
