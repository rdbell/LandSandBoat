#include "test_aicontainer_tick_controller_6361.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer TickController 6361 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for AI Tick controller admission (slice 6361).
// Go host half drives controllerTick via aicontainer.TickController.
auto runAicontainerTickController6361SelfTests() -> bool
{
    using aicontainerhelpers::ShouldClearPathOnReset;
    using aicontainerhelpers::ShouldTickController;
    using aicontainerhelpers::ShouldTickFollowPath;

    bool ok = true;

    ok = expect(!ShouldTickController(false, true), "no controller") && ok;
    ok = expect(!ShouldTickController(true, false), "!canUpdate") && ok;
    ok = expect(!ShouldTickController(false, false), "both false") && ok;
    ok = expect(ShouldTickController(true, true), "admit") && ok;
    // Mutual exclusion compose: controller present blocks pathing.
    ok = expect(!ShouldTickFollowPath(true, true, false), "ctrl blocks path") && ok;
    ok = expect(ShouldTickFollowPath(false, true, false), "no ctrl admits path") && ok;
    ok = expect(ShouldClearPathOnReset(true), "6360 residual") && ok;

    return ok;
}
