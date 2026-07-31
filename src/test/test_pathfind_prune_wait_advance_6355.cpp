#include "test_pathfind_prune_wait_advance_6355.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind prune wait advance 6355 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for original moving/rotation, prune pair, advance (6355).
auto runPathfindPruneWaitAdvance6355SelfTests() -> bool
{
    using pathfindstatushelpers::AdvancedCurrentPoint;
    using pathfindstatushelpers::ClearedWaiting;
    using pathfindstatushelpers::OriginalPointMoving;
    using pathfindstatushelpers::OriginalPointRotation;
    using pathfindstatushelpers::OriginalPointX;
    using pathfindstatushelpers::ShouldPruneHasPair;

    bool ok = true;

    ok = expect(OriginalPointMoving(0) == 0, "moving 0") && ok;
    ok = expect(OriginalPointMoving(0x28) == 0x28, "moving 0x28") && ok;
    ok = expect(OriginalPointRotation(0) == 0, "rot 0") && ok;
    ok = expect(OriginalPointRotation(64) == 64, "rot 64") && ok;
    ok = expect(OriginalPointRotation(255) == 255, "rot 255") && ok;
    ok = expect(!ShouldPruneHasPair(0), "pair 0") && ok;
    ok = expect(!ShouldPruneHasPair(1), "pair 1") && ok;
    ok = expect(ShouldPruneHasPair(2), "pair 2") && ok;
    ok = expect(ShouldPruneHasPair(50), "pair 50") && ok;
    // The production point count is std::size_t; preserve unsigned conversion
    // for a synthetic negative host value at the pair threshold.
    ok = expect(ShouldPruneHasPair(-1), "signed wrap pair") && ok;
    ok = expect(AdvancedCurrentPoint(0) == 1, "advance 0") && ok;
    ok = expect(AdvancedCurrentPoint(9) == 10, "advance 9") && ok;
    ok = expect(AdvancedCurrentPoint(-1) == 0, "advance -1") && ok;
    ok = expect(OriginalPointX(1.f) == 1.f && !ClearedWaiting(), "6354 residual") && ok;

    return ok;
}
