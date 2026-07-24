#include "test_pathfind_step_snap_6352.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind step snap 6352 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for StepTo exact-snap and FindPath cursor (slice 6352).
auto runPathfindStepSnap6352SelfTests() -> bool
{
    using pathfindstatushelpers::FindPathCursor;
    using pathfindstatushelpers::RestartedCurrentPoint;
    using pathfindstatushelpers::ShouldSnapshotPatrol;
    using pathfindstatushelpers::ShouldSnapExact;
    using pathfindstatushelpers::SnapDistanceMoved;

    bool ok = true;

    ok = expect(ShouldSnapExact(0.f), "exact 0") && ok;
    ok = expect(!ShouldSnapExact(0.1f), "exact 0.1") && ok;
    ok = expect(!ShouldSnapExact(-1.f), "exact -1") && ok;
    ok = expect(SnapDistanceMoved(10.f, 2.f) == 8.f, "snap moved") && ok;
    ok = expect(SnapDistanceMoved(1.f, 1.f) == 0.f, "snap zero") && ok;
    ok = expect(FindPathCursor() == 0, "find cursor") && ok;
    ok = expect(RestartedCurrentPoint() == 0 && ShouldSnapshotPatrol(true), "6351 residual") && ok;

    return ok;
}
