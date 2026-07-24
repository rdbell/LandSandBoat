#include "test_pathfind_follow_iterate_6356.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind follow iterate 6356 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for FollowPath iterate / turn advance / wait deadline (6356).
auto runPathfindFollowIterate6356SelfTests() -> bool
{
    using pathfindstatushelpers::AdvancedCurrentPoint;
    using pathfindstatushelpers::AdvancedCurrentTurn;
    using pathfindstatushelpers::ShouldIteratePathPoint;
    using pathfindstatushelpers::ShouldPruneHasPair;
    using pathfindstatushelpers::WaitDeadlineFrom;

    bool ok = true;

    ok = expect(!ShouldIteratePathPoint(0, 0), "iterate 0/0") && ok;
    ok = expect(ShouldIteratePathPoint(0, 1), "iterate 0/1") && ok;
    ok = expect(!ShouldIteratePathPoint(1, 1), "iterate 1/1") && ok;
    ok = expect(ShouldIteratePathPoint(1, 2), "iterate 1/2") && ok;
    ok = expect(AdvancedCurrentTurn(0) == 1, "turn 0") && ok;
    ok = expect(AdvancedCurrentTurn(4) == 5, "turn 4") && ok;
    ok = expect(WaitDeadlineFrom(10, 5) == 15, "deadline") && ok;
    ok = expect(WaitDeadlineFrom(0, 0) == 0, "deadline zero") && ok;
    ok = expect(AdvancedCurrentPoint(2) == 3 && ShouldPruneHasPair(2), "6355 residual") && ok;

    return ok;
}
