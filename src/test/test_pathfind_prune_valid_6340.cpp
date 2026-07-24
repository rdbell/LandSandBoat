#include "test_pathfind_prune_valid_6340.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind prune/valid 6340 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for PrunePathWithin + ValidPosition (slice 6340).
auto runPathfindPruneValid6340SelfTests() -> bool
{
    using pathfindstatushelpers::LimitDistanceReached;
    using pathfindstatushelpers::ShouldContinuePrune;
    using pathfindstatushelpers::ShouldPrunePath;
    using pathfindstatushelpers::ShouldTruncatePathPoints;
    using pathfindstatushelpers::ValidPosition;

    bool ok = true;

    ok = expect(ShouldPrunePath(true) && !ShouldPrunePath(false), "prune admission") && ok;
    ok = expect(!ShouldContinuePrune(1, 0.f, 10.f), "single point") && ok;
    ok = expect(ShouldContinuePrune(3, 5.f, 10.f), "within continues") && ok;
    ok = expect(!ShouldContinuePrune(3, 11.f, 10.f), "beyond stops") && ok;
    ok = expect(ValidPosition(true) && !ValidPosition(false), "valid inject") && ok;
    ok = expect(ShouldTruncatePathPoints(51, 50, false), "6339 residual") && ok;
    ok = expect(!LimitDistanceReached(0.f, 1.f), "6339 residual limit") && ok;

    return ok;
}
