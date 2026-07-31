#include "test_pathfind_find_path_gates_6348.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind find-path gates 6348 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for FindPath/FindClosest/FindRandom gates (slice 6348).
auto runPathfindFindPathGates6348SelfTests() -> bool
{
    using pathfindstatushelpers::ArePositionsClose;
    using pathfindstatushelpers::DistanceFromPointValue;
    using pathfindstatushelpers::FindPathSucceeded;
    using pathfindstatushelpers::RandomPathHasTurns;
    using pathfindstatushelpers::RandomPathPolyRadius;
    using pathfindstatushelpers::ShouldClearAfterFailedPath;

    bool ok = true;

    ok = expect(ArePositionsClose(0.999f) && !ArePositionsClose(1.0f), "close boundary") && ok;
    ok = expect(FindPathSucceeded(1) && !FindPathSucceeded(0), "path success") && ok;
    // The production point count is std::size_t; preserve unsigned conversion
    // for a synthetic negative host value at the non-empty boundary.
    ok = expect(FindPathSucceeded(-1), "signed wrap path success") && ok;
    ok = expect(RandomPathPolyRadius(10.f) == 1.f, "poly radius") && ok;
    ok = expect(ShouldClearAfterFailedPath(false) && !ShouldClearAfterFailedPath(true), "clear after fail") && ok;
    ok = expect(RandomPathHasTurns(1) && !RandomPathHasTurns(0), "has turns") && ok;
    // The production turn count is std::size_t; preserve unsigned conversion
    // for a synthetic negative host value at the non-empty boundary.
    ok = expect(RandomPathHasTurns(-1), "signed wrap has turns") && ok;
    ok = expect(DistanceFromPointValue(3.f) == 3.f, "6347 residual") && ok;

    return ok;
}
