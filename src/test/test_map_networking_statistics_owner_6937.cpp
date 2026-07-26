/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_map_networking_statistics_owner_6937.h"

#include <iostream>

#include "common/scheduler.h"
#include "map/map_config.h"
#include "map/map_networking.h"
#include "map/map_statistics.h"

namespace
{

auto expect(const bool actual, const char* label) -> bool
{
    if (!actual)
    {
        std::cerr << "map networking statistics-owner self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMapNetworkingStatisticsOwner6937SelfTests() -> bool
{
    auto config         = MapConfig{};
    config.isTestServer = true;

    Scheduler     scheduler(1);
    MapStatistics statistics;
    MapNetworking networking(scheduler, statistics, config);

    statistics.increment(MapStatistics::Key::TotalPacketsSentPerTick, 4);
    networking.flushStatistics();

    // flushStatistics operates on the MapStatistics reference received by the
    // MapNetworking constructor, and flush() clears its known counters.
    return expect(statistics.get(MapStatistics::Key::TotalPacketsSentPerTick) == 0, "flushes owned statistics");
}
