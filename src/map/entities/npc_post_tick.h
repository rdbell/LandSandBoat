#pragma once

#include "common/cbasetypes.h"
#include "common/timer.h"

#include <chrono>

namespace npcentity
{

struct PostTickPlan
{
    bool              sendUpdate{};
    timer::time_point nextUpdateTimer{};
};

// Plans CNpcEntity::PostTick's update-mask flush. The caller owns the zone
// packet send and clears its mask only when sendUpdate is true.
inline auto PlanPostTick(const bool hasZone, const uint8 updateMask, const bool isDisappearing, const timer::time_point now, const timer::time_point nextUpdateTimer) -> PostTickPlan
{
    if (!hasZone || updateMask == 0 || isDisappearing || now <= nextUpdateTimer)
    {
        return {};
    }

    return { true, now + std::chrono::milliseconds(250) };
}

} // namespace npcentity
