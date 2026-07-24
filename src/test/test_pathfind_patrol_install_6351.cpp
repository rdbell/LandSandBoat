#include "test_pathfind_patrol_install_6351.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind patrol install 6351 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for PathFlags/patrol snapshot/restart injects (slice 6351).
auto runPathfindPatrolInstall6351SelfTests() -> bool
{
    using pathfindstatushelpers::ArrivedOnPoint;
    using pathfindstatushelpers::ClearedPatrolFlags;
    using pathfindstatushelpers::LeavingPoint;
    using pathfindstatushelpers::PathFlagsValue;
    using pathfindstatushelpers::PatrolFlagsValue;
    using pathfindstatushelpers::RestartedCurrentPoint;
    using pathfindstatushelpers::RestartedCurrentTurn;
    using pathfindstatushelpers::ShouldSnapshotPatrol;

    bool ok = true;

    ok = expect(PathFlagsValue(0) == 0, "pathFlags 0") && ok;
    ok = expect(PathFlagsValue(0x20) == 0x20, "pathFlags patrol") && ok;
    ok = expect(PathFlagsValue(0xFF) == 0xFF, "pathFlags max") && ok;
    ok = expect(!ShouldSnapshotPatrol(false), "snapshot false") && ok;
    ok = expect(ShouldSnapshotPatrol(true), "snapshot true") && ok;
    ok = expect(PatrolFlagsValue(0x20) == 0x20, "patrolFlags") && ok;
    ok = expect(ClearedPatrolFlags() == 0, "cleared patrolFlags") && ok;
    ok = expect(RestartedCurrentPoint() == 0, "restart point") && ok;
    ok = expect(RestartedCurrentTurn() == 0, "restart turn") && ok;
    ok = expect(!LeavingPoint() && ArrivedOnPoint(), "6350 residual") && ok;

    return ok;
}
