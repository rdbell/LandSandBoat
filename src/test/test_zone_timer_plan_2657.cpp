#include "test_zone_timer_plan_2657.h"

#include <chrono>
#include <iostream>

#include "map/zone_capacity.h"

auto runZoneTimerPlan2657SelfTests() -> bool
{
    const auto testServer = zonehelpers::PlanZoneTimers(true);
    const auto runtime    = zonehelpers::PlanZoneTimers(false);
    const auto ok         = !testServer.install &&
                            runtime.install &&
                            runtime.logicInterval == std::chrono::milliseconds(400) &&
                            runtime.triggerAreaInterval == std::chrono::milliseconds(200);
    if (!ok)
    {
        std::cerr << "zone timer plan self-test failed\n";
    }
    return ok;
}
