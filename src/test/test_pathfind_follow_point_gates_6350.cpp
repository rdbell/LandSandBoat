#include "test_pathfind_follow_point_gates_6350.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind follow point gates 6350 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for FollowPath wait/rotation/run/roam injects (slice 6350).
auto runPathfindFollowPointGates6350SelfTests() -> bool
{
    using pathfindstatushelpers::ArrivedOnPoint;
    using pathfindstatushelpers::ClearedOnPoint;
    using pathfindstatushelpers::CompletedOnPoint;
    using pathfindstatushelpers::HasActiveWaypointWait;
    using pathfindstatushelpers::LeavingPoint;
    using pathfindstatushelpers::RoamFlagsValue;
    using pathfindstatushelpers::ShouldApplyPointRotation;
    using pathfindstatushelpers::ShouldStepWithRun;

    bool ok = true;

    ok = expect(!HasActiveWaypointWait(false), "wait false") && ok;
    ok = expect(HasActiveWaypointWait(true), "wait true") && ok;
    ok = expect(!ShouldApplyPointRotation(false), "rotation false") && ok;
    ok = expect(ShouldApplyPointRotation(true), "rotation true") && ok;
    ok = expect(!ShouldStepWithRun(false), "run false") && ok;
    ok = expect(ShouldStepWithRun(true), "run true") && ok;
    ok = expect(RoamFlagsValue(0) == 0, "roam 0") && ok;
    ok = expect(RoamFlagsValue(0x40) == 0x40, "roam 0x40") && ok;
    ok = expect(RoamFlagsValue(0xFFFF) == 0xFFFF, "roam max") && ok;
    ok = expect(CompletedOnPoint(), "completed onPoint") && ok;
    ok = expect(ArrivedOnPoint(), "arrived onPoint") && ok;
    ok = expect(!LeavingPoint(), "leaving onPoint") && ok;
    ok = expect(ClearedOnPoint(), "6349 residual") && ok;

    return ok;
}
