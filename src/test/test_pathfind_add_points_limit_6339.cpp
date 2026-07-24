#include "test_pathfind_add_points_limit_6339.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind add-points/limit 6339 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for AddPoints truncation + LimitDistance stop (6339).
auto runPathfindAddPointsLimit6339SelfTests() -> bool
{
    using pathfindstatushelpers::IsWaterTerrain;
    using pathfindstatushelpers::LimitDistanceReached;
    using pathfindstatushelpers::ShouldTruncatePathPoints;

    bool ok = true;

    ok = expect(!ShouldTruncatePathPoints(50, 50, false), "equal → no truncate") && ok;
    ok = expect(ShouldTruncatePathPoints(51, 50, false), "over → truncate") && ok;
    ok = expect(!ShouldTruncatePathPoints(100, 50, true), "patrol never truncates") && ok;

    ok = expect(!LimitDistanceReached(0.f, 100.f), "max 0 disables") && ok;
    ok = expect(LimitDistanceReached(10.f, 10.f), "at max") && ok;
    ok = expect(LimitDistanceReached(10.f, 11.f), "over max") && ok;
    ok = expect(!LimitDistanceReached(10.f, 9.9f), "under max") && ok;

    ok = expect(!IsWaterTerrain(false, false), "6338 residual") && ok;

    return ok;
}
