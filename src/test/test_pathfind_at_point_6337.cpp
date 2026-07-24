#include "test_pathfind_at_point_6337.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind AtPoint 6337 self-test failed: " << label << '\n';
    }
    return condition;
}

auto nearEq(const float a, const float b) -> bool
{
    return std::fabs(a - b) < 1e-6f;
}

} // namespace

// Pure dual-wire suite for CPathFind::AtPoint threshold (slice 6337).
auto runPathfindAtPoint6337SelfTests() -> bool
{
    using pathfindstatushelpers::AtPointThreshold;
    using pathfindstatushelpers::IsFollowingPath;
    using pathfindstatushelpers::OnPoint;

    bool ok = true;

    ok = expect(nearEq(AtPointThreshold(0.f), 0.1f), "zero → 0.1") && ok;
    ok = expect(nearEq(AtPointThreshold(1.f), 1.2f), "1 → 1.2") && ok;
    ok = expect(nearEq(AtPointThreshold(2.5f), 2.7f), "2.5 → 2.7") && ok;
    ok = expect(!IsFollowingPath(0) && OnPoint(true), "6336 residual") && ok;

    return ok;
}
