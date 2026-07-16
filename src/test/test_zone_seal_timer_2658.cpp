#include "test_zone_seal_timer_2658.h"
#include "map/zone_capacity.h"
#include <chrono>
#include <iostream>

auto runZoneSealTimer2658SelfTests() -> bool
{
    using namespace std::chrono_literals;
    const auto n  = uint32{ 100 };
    const auto ok = !zonehelpers::PlanSealTimerExpiry(false, true, 11s, n) && !zonehelpers::PlanSealTimerExpiry(true, false, 11s, n) && !zonehelpers::PlanSealTimerExpiry(true, true, 10s, n) && zonehelpers::PlanSealTimerExpiry(true, true, 10999ms, n) == 110;
    if (!ok)
        std::cerr << "zone seal timer self-test failed\n";
    return ok;
}
