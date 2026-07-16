#include "test_zone_idle_shutdown_2652.h"

#include <chrono>
#include <iostream>

#include "map/zone_capacity.h"

using namespace std::chrono_literals;

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone idle shutdown self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runZoneIdleShutdown2652SelfTests() -> bool
{
    const auto emptySince = timer::time_point{} + 1min;
    bool       ok         = true;
    ok                    = expect(!zonehelpers::ShouldStopZoneTimers(false, true, emptySince, emptySince + 6s, true), "zone timer required") && ok;
    ok                    = expect(!zonehelpers::ShouldStopZoneTimers(true, false, emptySince, emptySince + 6s, true), "characters prevent shutdown") && ok;
    ok                    = expect(!zonehelpers::ShouldStopZoneTimers(true, true, emptySince, emptySince + 5s, true), "five-second equality retained") && ok;
    ok                    = expect(!zonehelpers::ShouldStopZoneTimers(true, true, emptySince, emptySince + 6s, false), "mobs away prevent shutdown") && ok;
    return expect(zonehelpers::ShouldStopZoneTimers(true, true, emptySince, emptySince + 5s + 1ms, true), "post-delay empty zone stops timers") && ok;
}
