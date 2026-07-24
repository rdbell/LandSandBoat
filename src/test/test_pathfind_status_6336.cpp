#include "test_pathfind_status_6336.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind status 6336 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CPathFind follow/status injects (slice 6336).
auto runPathfindStatus6336SelfTests() -> bool
{
    using pathfindstatushelpers::IsFollowingPath;
    using pathfindstatushelpers::IsFollowingScriptedPath;
    using pathfindstatushelpers::IsPatrolling;
    using pathfindstatushelpers::OnPoint;

    bool ok = true;

    ok = expect(!IsFollowingPath(0) && IsFollowingPath(1) && IsFollowingPath(3), "follow by count") && ok;
    ok = expect(!IsFollowingScriptedPath(false, true) && !IsFollowingScriptedPath(true, false), "scripted requires both") && ok;
    ok = expect(IsFollowingScriptedPath(true, true), "scripted both true") && ok;
    ok = expect(!IsPatrolling(false) && IsPatrolling(true), "patrol flag") && ok;
    ok = expect(!OnPoint(false) && OnPoint(true), "onPoint flag") && ok;

    return ok;
}
