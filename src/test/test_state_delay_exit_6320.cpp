#include "test_state_delay_exit_6320.h"

#include "map/ai/states/state_delay_exit.h"
#include "map/ai/states/state_set_target.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state DelayExitTime 6320 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for CState::DelayExitTime (slice 6320).
auto runStateDelayExit6320SelfTests() -> bool
{
    using statehelpers::delayExitPlan;
    using statehelpers::ShouldUpdateTarget;

    bool ok = true;

    const auto entry = timer::time_point{};
    const auto plan  = delayExitPlan(entry, 250ms);
    ok               = expect(plan.entryTime == entry + 250ms, "entryTime += 250ms") && ok;
    ok               = expect(plan.wasDelayed, "wasDelayed true") && ok;

    const auto zero = delayExitPlan(entry, 0ms);
    ok              = expect(zero.entryTime == entry && zero.wasDelayed, "zero delay still marks delayed") && ok;

    // Residual: 6317 SetTarget still holds.
    ok = expect(!ShouldUpdateTarget(true, 1, 1, 1), "6317 residual: stable target") && ok;
    ok = expect(ShouldUpdateTarget(false, 0, 0, 0), "6317 residual: no target") && ok;

    // Accumulate delays.
    const auto mid = delayExitPlan(entry, 1000ms);
    const auto end = delayExitPlan(mid.entryTime, 500ms);
    ok             = expect(end.entryTime == entry + 1500ms && end.wasDelayed, "accumulate delays") && ok;

    return ok;
}
