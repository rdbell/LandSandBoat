#include "test_map_networking_flush_statistics_6921.h"

#include "map/map_networking_flush_statistics.h"

#include <array>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking flush statistics 6921 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingFlushStatistics6921SelfTests() -> bool
{
    using mapnetworkingflushstatisticshelpers::MakePlan;
    using mapnetworkingflushstatisticshelpers::ZoneSnapshot;

    bool ok = true;

    auto plan = MakePlan(std::array<ZoneSnapshot, 0>{});
    ok        = expect(plan.activeZoneCount == 0 && plan.playerCount == 0 && plan.mobCount == 0 && plan.dynamicTargIdUsagePercent == 0,
                 "no active zones produces zero counters") &&
         ok;

    plan = MakePlan(std::array{
        ZoneSnapshot{ .active = false, .playerCount = 99, .mobCount = 88, .dynamicTargIdCount = 77 },
        ZoneSnapshot{ .active = true, .playerCount = 3, .mobCount = 5, .dynamicTargIdCount = 100 },
        ZoneSnapshot{ .active = true, .playerCount = 4, .mobCount = 6, .dynamicTargIdCount = 200 },
    });
    ok = expect(plan.activeZoneCount == 2 && plan.playerCount == 7 && plan.mobCount == 11 && plan.dynamicTargIdUsagePercent == 29,
                "only active zones contribute to aggregate counters") &&
         ok;

    return ok;
}
