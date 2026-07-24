#include "test_pathfind_path_flags_6345.h"

#include "map/ai/helpers/pathfind_status_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pathfind path-flags 6345 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for reverse/clear/worm/moving (slice 6345).
auto runPathfindPathFlags6345SelfTests() -> bool
{
    using pathfindstatushelpers::MovingDelta;
    using pathfindstatushelpers::ShouldBlockNonScriptPath;
    using pathfindstatushelpers::ShouldClearBeforePath;
    using pathfindstatushelpers::ShouldReversePoints;
    using pathfindstatushelpers::WormStepSpeedOverride;
    using pathfindstatushelpers::WrapMoving;

    bool ok = true;

    ok = expect(ShouldReversePoints(true) && !ShouldReversePoints(false), "reverse") && ok;
    ok = expect(ShouldClearBeforePath(true) && !ShouldClearBeforePath(false), "clear") && ok;

    float wormSpeed = 0.f;
    ok = expect(WormStepSpeedOverride(true, true, wormSpeed) && wormSpeed == 20.f, "worm override") && ok;
    ok = expect(!WormStepSpeedOverride(false, true, wormSpeed), "no worm when speed non-zero") && ok;
    ok = expect(!WormStepSpeedOverride(true, false, wormSpeed), "no worm when not worm flag") && ok;

    ok = expect(MovingDelta(true) == 0x28 && MovingDelta(false) == 0x35, "moving delta") && ok;
    ok = expect(WrapMoving(0x1FFF, false) == 0x34, "wrap moving") && ok;
    ok = expect(ShouldBlockNonScriptPath(true, true, false), "6344 residual") && ok;

    return ok;
}
