#include "test_aicontainer_reset_trigger_pathing_6360.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Reset/Trigger pathing 6360 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Reset path clear / Trigger pathing pause (slice 6360).
// Go host half drives pathfind.Clear via aicontainer.Reset.
auto runAicontainerResetTriggerPathing6360SelfTests() -> bool
{
    using aicontainerhelpers::ShouldClearPathOnReset;
    using aicontainerhelpers::ShouldPausePathingOnTrigger;
    using aicontainerhelpers::ShouldResetControllerOnReset;
    using aicontainerhelpers::ShouldTickFollowPath;

    bool ok = true;

    ok = expect(!ShouldClearPathOnReset(false), "clear false") && ok;
    ok = expect(ShouldClearPathOnReset(true), "clear true") && ok;
    ok = expect(!ShouldResetControllerOnReset(false), "ctrl false") && ok;
    ok = expect(ShouldResetControllerOnReset(true), "ctrl true") && ok;
    ok = expect(!ShouldPausePathingOnTrigger(false, 1), "pause no path") && ok;
    ok = expect(!ShouldPausePathingOnTrigger(true, 0), "pause no stop") && ok;
    ok = expect(!ShouldPausePathingOnTrigger(true, 2), "pause stop!=1") && ok;
    ok = expect(ShouldPausePathingOnTrigger(true, 1), "pause admit") && ok;
    // Compose: paused blocks tick pathing (6359 residual).
    ok = expect(!ShouldTickFollowPath(false, true, ShouldPausePathingOnTrigger(true, 1)),
                "pause blocks tick pathing") &&
         ok;
    ok = expect(ShouldTickFollowPath(false, true, false), "6359 residual") && ok;

    return ok;
}
