#include "test_pathfind_resume_closest_6353.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind resume closest 6353 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for ResumePatrol closest-point injects (slice 6353).
auto runPathfindResumeClosest6353SelfTests() -> bool
{
    using pathfindstatushelpers::FindPathCursor;
    using pathfindstatushelpers::InitialClosestPatrolDistance;
    using pathfindstatushelpers::ResumePatrolCursor;
    using pathfindstatushelpers::ShouldSnapExact;
    using pathfindstatushelpers::ShouldUpdateClosestPatrol;

    bool ok = true;

    ok = expect(ResumePatrolCursor() == 0, "cursor 0") && ok;
    ok = expect(InitialClosestPatrolDistance() > 1.0e+37f, "seed FLT_MAX-ish") && ok;
    ok = expect(ShouldUpdateClosestPatrol(1.f, 2.f), "update nearer") && ok;
    ok = expect(!ShouldUpdateClosestPatrol(2.f, 1.f), "skip farther") && ok;
    ok = expect(!ShouldUpdateClosestPatrol(1.f, 1.f), "skip equal") && ok;
    ok = expect(FindPathCursor() == 0 && ShouldSnapExact(0.f), "6352 residual") && ok;

    return ok;
}
