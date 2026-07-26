#include "test_synth_start_7078.h"

#include "map/synth_start.h"

#include <iostream>

auto runSynthStart7078SelfTests() -> bool
{
    const auto unresolved = synthstarthelpers::MakePlan(false, false);
    const auto unclaimed  = synthstarthelpers::MakePlan(true, false);
    const auto started    = synthstarthelpers::MakePlan(true, true);
    const bool ok         = unresolved == synthstarthelpers::Plan{ .recordLastSynthTime = true } &&
                            unclaimed == synthstarthelpers::Plan{
                                .recordLastSynthTime = true,
                                .startTransaction    = true,
                                .warnClaimFailure    = true,
                                .sendCancelBadRecipe = true,
                            } &&
                            started == synthstarthelpers::Plan{
                                .recordLastSynthTime  = true,
                                .startTransaction     = true,
                                .consumeCrystal       = true,
                                .resolveResult        = true,
                                .setAnimationSynth    = true,
                                .setUpdateHP          = true,
                                .sendCharStatus       = true,
                                .startCharacterSynth  = true,
                                .broadcastSynthEffect = true,
                            } &&
                            synthstarthelpers::HighestRequiredCraft({}) == 0 && synthstarthelpers::HighestRequiredCraft({ 20, 60, 60, 5 }) == 50 && synthstarthelpers::HighestRequiredCraft({ 1, 2, 3, 4, 5, 6, 7, 80 }) == 56;
    if (!ok)
    {
        std::cerr << "synth start 7078 self-test failed\n";
    }
    return ok;
}
