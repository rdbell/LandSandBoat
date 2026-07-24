#pragma once

#include "common/timer.h"

// Pure CState::DelayExitTime field plan (slice 6320).
// Dual-wire of Go aistate.DelayExitPlan (internal/aistate/delay_exit.go).

namespace statehelpers
{

// DelayExitPlan: entryTime += delay; wasDelayed = true.
struct DelayExitPlan
{
    timer::time_point entryTime;
    bool              wasDelayed{ true };
};

inline auto delayExitPlan(const timer::time_point entryTime, const std::chrono::milliseconds delay) -> DelayExitPlan
{
    return DelayExitPlan{ entryTime + delay, true };
}

} // namespace statehelpers
