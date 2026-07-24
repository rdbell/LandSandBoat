#include "test_pathfind_follow_admissions_6343.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind follow admissions 6343 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for FollowPath careful/wait start (slice 6343).
auto runPathfindFollowAdmissions6343SelfTests() -> bool
{
    using pathfindstatushelpers::CarefulPathingValue;
    using pathfindstatushelpers::ShouldSnapCareful;
    using pathfindstatushelpers::ShouldStartWaypointWait;
    using pathfindstatushelpers::ShouldUpdateLookAt;

    bool ok = true;

    ok = expect(ShouldSnapCareful(true) && !ShouldSnapCareful(false), "snap careful") && ok;
    ok = expect(ShouldStartWaypointWait(true, false) && !ShouldStartWaypointWait(true, true), "start wait") && ok;
    ok = expect(!ShouldStartWaypointWait(false, false), "zero wait") && ok;
    ok = expect(CarefulPathingValue(true) && !CarefulPathingValue(false), "careful value") && ok;
    ok = expect(ShouldUpdateLookAt(false), "6342 residual") && ok;

    return ok;
}
