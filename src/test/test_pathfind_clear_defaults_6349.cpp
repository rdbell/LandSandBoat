#include "test_pathfind_clear_defaults_6349.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind Clear defaults 6349 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CPathFind::Clear sticky defaults (slice 6349).
auto runPathfindClearDefaults6349SelfTests() -> bool
{
    using pathfindstatushelpers::ArePositionsClose;
    using pathfindstatushelpers::ClearedCurrentPoint;
    using pathfindstatushelpers::ClearedCurrentTurn;
    using pathfindstatushelpers::ClearedDistanceFromPoint;
    using pathfindstatushelpers::ClearedDistanceMoved;
    using pathfindstatushelpers::ClearedMaxDistance;
    using pathfindstatushelpers::ClearedOnPoint;
    using pathfindstatushelpers::ClearedPathFlags;
    using pathfindstatushelpers::ClearedRoamFlags;

    bool ok = true;

    ok = expect(ClearedOnPoint(), "onPoint true") && ok;
    ok = expect(ClearedCurrentPoint() == 0, "currentPoint 0") && ok;
    ok = expect(ClearedDistanceFromPoint() == 0.f, "distanceFromPoint 0") && ok;
    ok = expect(ClearedMaxDistance() == 0.f, "maxDistance 0") && ok;
    ok = expect(ClearedDistanceMoved() == 0.f, "distanceMoved 0") && ok;
    ok = expect(ClearedCurrentTurn() == 0, "currentTurn 0") && ok;
    ok = expect(ClearedPathFlags() == 0, "pathFlags 0") && ok;
    ok = expect(ClearedRoamFlags() == 0, "roamFlags 0") && ok;
    ok = expect(ArePositionsClose(0.5f), "6348 residual") && ok;

    return ok;
}
