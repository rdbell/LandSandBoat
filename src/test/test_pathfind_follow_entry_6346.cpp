#include "test_pathfind_follow_entry_6346.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind follow entry 6346 self-test failed: " << label << '\n';
    }
    return condition;
}

auto nearEq(const float a, const float b) -> bool
{
    return std::fabs(a - b) < 1e-6f;
}

} // namespace

// Pure dual-wire suite for FollowPath entry / vertical step (slice 6346).
auto runPathfindFollowEntry6346SelfTests() -> bool
{
    using pathfindstatushelpers::ClampVerticalStep;
    using pathfindstatushelpers::HasDestination;
    using pathfindstatushelpers::ShouldFollowPath;
    using pathfindstatushelpers::ShouldReversePoints;
    using pathfindstatushelpers::ShouldStepVertical;

    bool ok = true;

    ok = expect(ShouldFollowPath(true) && !ShouldFollowPath(false), "follow entry") && ok;
    ok = expect(!ShouldStepVertical(0.5f) && ShouldStepVertical(0.51f), "vertical boundary") && ok;
    ok = expect(HasDestination(1) && !HasDestination(0), "destination") && ok;
    // The production point count is std::size_t; preserve unsigned conversion
    // for a synthetic negative host value at the non-empty boundary.
    ok = expect(HasDestination(-1), "signed wrap has destination") && ok;
    ok = expect(nearEq(ClampVerticalStep(0.f, 1.f, 3.f), 1.f), "clamp low") && ok;
    ok = expect(nearEq(ClampVerticalStep(2.f, 1.f, 3.f), 2.f), "clamp mid") && ok;
    ok = expect(nearEq(ClampVerticalStep(5.f, 1.f, 3.f), 3.f), "clamp high") && ok;
    ok = expect(nearEq(ClampVerticalStep(0.f, 3.f, 1.f), 1.f), "clamp descending low") && ok;
    ok = expect(ShouldReversePoints(true), "6345 residual") && ok;

    return ok;
}
