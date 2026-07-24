#include "test_pathfind_path_to_gates_6344.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind PathTo/StepTo 6344 self-test failed: " << label << '\n';
    }
    return condition;
}

auto nearEq(const float a, const float b) -> bool
{
    return std::fabs(a - b) < 1e-6f;
}

} // namespace

// Pure dual-wire suite for PathTo/StepTo gates (slice 6344).
auto runPathfindPathToGates6344SelfTests() -> bool
{
    using pathfindstatushelpers::CarefulPathingValue;
    using pathfindstatushelpers::ShouldBlockNonScriptPath;
    using pathfindstatushelpers::ShouldSnapToTarget;
    using pathfindstatushelpers::ShouldUseWallhackPath;
    using pathfindstatushelpers::StepDistance;

    bool ok = true;

    ok = expect(!ShouldBlockNonScriptPath(false, true, false), "not following") && ok;
    ok = expect(!ShouldBlockNonScriptPath(true, false, false), "non-script current") && ok;
    ok = expect(!ShouldBlockNonScriptPath(true, true, true), "new script") && ok;
    ok = expect(ShouldBlockNonScriptPath(true, true, false), "block non-script over script") && ok;

    ok = expect(ShouldUseWallhackPath(true) && !ShouldUseWallhackPath(false), "wallhack") && ok;
    ok = expect(nearEq(StepDistance(100.f, false), 2.5f), "walk step") && ok;
    ok = expect(nearEq(StepDistance(100.f, true), 2.f), "run step") && ok;
    ok = expect(ShouldSnapToTarget(1.f, 0.f, 1.f) && !ShouldSnapToTarget(3.f, 0.f, 1.f), "snap") && ok;
    ok = expect(CarefulPathingValue(true), "6343 residual") && ok;

    return ok;
}
