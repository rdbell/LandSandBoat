#include "test_synth_start_plan.h"

#include "map/synth_start.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "synth start plan self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSynthStartPlanSelfTests() -> bool
{
    using synthstarthelpers::MakePlan;
    bool ok = true;

    const auto unresolved = MakePlan(false, false);
    ok = expect(unresolved.recordLastSynthTime && !unresolved.startTransaction, "timestamp before unresolved recipe") && ok;

    const auto failedClaim = MakePlan(true, false);
    ok = expect(failedClaim.recordLastSynthTime && failedClaim.startTransaction && failedClaim.warnClaimFailure && failedClaim.sendCancelBadRecipe && !failedClaim.consumeCrystal, "timestamp before failed claim") && ok;

    const auto started = MakePlan(true, true);
    ok = expect(started.recordLastSynthTime && started.consumeCrystal && started.startCharacterSynth && started.broadcastSynthEffect, "successful synthesis plan") && ok;
    return ok;
}
