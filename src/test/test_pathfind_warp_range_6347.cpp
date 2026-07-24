#include "test_pathfind_warp_range_6347.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind warp/range 6347 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for PathInRange/LimitDistance/WarpTo (slice 6347).
auto runPathfindWarpRange6347SelfTests() -> bool
{
    using pathfindstatushelpers::DistanceFromPointValue;
    using pathfindstatushelpers::LimitDistanceValue;
    using pathfindstatushelpers::ShouldFollowPath;
    using pathfindstatushelpers::ShouldNotifyZoneOnMove;
    using pathfindstatushelpers::WarpMovingReset;

    bool ok = true;

    ok = expect(DistanceFromPointValue(3.5f) == 3.5f, "range identity") && ok;
    ok = expect(LimitDistanceValue(7.f) == 7.f, "limit identity") && ok;
    ok = expect(WarpMovingReset() == 0, "warp moving") && ok;
    ok = expect(ShouldNotifyZoneOnMove(true) && !ShouldNotifyZoneOnMove(false), "zone notify") && ok;
    ok = expect(ShouldFollowPath(true), "6346 residual") && ok;

    return ok;
}
