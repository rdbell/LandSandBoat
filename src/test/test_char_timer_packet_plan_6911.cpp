#include "test_char_timer_packet_plan_6911.h"

#include "map/char_timer_packet_plan.h"

#include <iostream>

auto runCharTimerPacketPlan6911SelfTests() -> bool
{
    using timerpackethelpers::Action;
    using timerpackethelpers::BuildClearPlan;
    using timerpackethelpers::BuildTimerPlan;
    using timerpackethelpers::Plan;

    const bool ok = BuildTimerPlan(42) == Plan{ Action::SendTimer, 42 } &&
                    BuildClearPlan() == Plan{ Action::ClearTimer, 0 };

    if (!ok)
    {
        std::cerr << "timer packet plan 6911 self-test failed\n";
    }

    return ok;
}
