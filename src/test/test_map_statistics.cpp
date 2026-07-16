/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_map_statistics.h"

#include "map_statistics.h"

#include <array>
#include <iostream>
#include <string_view>

namespace
{

using Key = MapStatistics::Key;

constexpr auto knownKeys = std::array{
    Key::TotalPacketsToSendPerTick,
    Key::TotalPacketsSentPerTick,
    Key::TotalPacketsDelayedPerTick,
    Key::TasksTickTime,
    Key::NetworkTickTime,
    Key::TotalTickTime,
    Key::TickDiffTime,
    Key::ActiveZones,
    Key::ConnectedPlayers,
    Key::ActiveMobs,
    Key::DynamicTargIdUsagePercent,
};

auto expectInt64(int64 actual, int64 expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map statistics self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectLabel(Key key, std::string_view expected, const char* label) -> bool
{
    const auto actual = std::string_view{ MapStatistics::toString(key) };
    if (actual != expected)
    {
        std::cerr << "map statistics self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectKnownZeroes(MapStatistics& stats, const char* label) -> bool
{
    bool ok = true;
    for (const auto key : knownKeys)
    {
        ok = expectInt64(stats.get(key), 0, label) && ok;
    }
    return ok;
}

auto testMapStatisticsLabels() -> bool
{
    bool ok = true;
    ok      = expectLabel(Key::TotalPacketsToSendPerTick, "Total Packets To Send Per Tick", "packets queued label") && ok;
    ok      = expectLabel(Key::TotalPacketsSentPerTick, "Total Packets Sent Per Tick", "packets sent label") && ok;
    ok      = expectLabel(Key::TotalPacketsDelayedPerTick, "Total Packets Delayed Per Tick", "packets delayed label") && ok;
    ok      = expectLabel(Key::TasksTickTime, "Tasks Tick Time (ms)", "tasks tick label") && ok;
    ok      = expectLabel(Key::NetworkTickTime, "Network Tick Time (ms)", "network tick label") && ok;
    ok      = expectLabel(Key::TotalTickTime, "Total Tick Time (ms)", "total tick label") && ok;
    ok      = expectLabel(Key::TickDiffTime, "Tick Diff/Sleep Time (ms)", "tick diff label") && ok;
    ok      = expectLabel(Key::ActiveZones, "Active Zones (Process)", "active zones label") && ok;
    ok      = expectLabel(Key::ConnectedPlayers, "Connected Players (Process)", "connected players label") && ok;
    ok      = expectLabel(Key::ActiveMobs, "Active Mobs (Process)", "active mobs label") && ok;
    ok      = expectLabel(Key::DynamicTargIdUsagePercent, "Dynamic TargID Usage (%)", "dynamic targid label") && ok;
    ok      = expectLabel(static_cast<Key>(9999), "Unknown", "unknown key label") && ok;
    return ok;
}

auto testMapStatisticsCounters() -> bool
{
    MapStatistics stats;

    bool ok = true;
    ok      = expectKnownZeroes(stats, "default known key value") && ok;
    ok      = expectInt64(stats.get(static_cast<Key>(9999)), 0, "unknown key default value") && ok;

    stats.set(Key::ConnectedPlayers, 7);
    stats.increment(Key::ConnectedPlayers, 5);
    stats.decrement(Key::ConnectedPlayers, 3);
    ok = expectInt64(stats.get(Key::ConnectedPlayers), 9, "set increment decrement") && ok;

    stats.increment(Key::TotalPacketsSentPerTick, 4);
    stats.decrement(Key::TotalPacketsSentPerTick, 10);
    ok = expectInt64(stats.get(Key::TotalPacketsSentPerTick), -6, "negative counter values") && ok;
    return ok;
}

auto testMapStatisticsTimingAggregation() -> bool
{
    MapStatistics stats;

    // Timing samples are signed millisecond counters. Each key aggregates
    // independently, and decrement uses the same signed arithmetic path as the
    // packet counters.
    stats.increment(Key::TasksTickTime, 7);
    stats.increment(Key::TasksTickTime, 5);
    stats.increment(Key::NetworkTickTime, 3);
    stats.decrement(Key::NetworkTickTime, 8);
    stats.set(Key::TotalTickTime, 19);
    stats.increment(Key::TickDiffTime, 4);

    bool ok = true;
    ok      = expectInt64(stats.get(Key::TasksTickTime), 12, "tasks timing samples accumulate") && ok;
    ok      = expectInt64(stats.get(Key::NetworkTickTime), -5, "network timing subtraction remains signed") && ok;
    ok      = expectInt64(stats.get(Key::TotalTickTime), 19, "total timing assignment") && ok;
    ok      = expectInt64(stats.get(Key::TickDiffTime), 4, "tick diff timing independent") && ok;

    stats.print();
    ok = expectInt64(stats.get(Key::TasksTickTime), 0, "tasks timing reset after report") && ok;
    ok = expectInt64(stats.get(Key::NetworkTickTime), 0, "network timing reset after report") && ok;
    ok = expectInt64(stats.get(Key::TotalTickTime), 0, "total timing reset after report") && ok;
    ok = expectInt64(stats.get(Key::TickDiffTime), 0, "tick diff timing reset after report") && ok;
    return ok;
}

auto testMapStatisticsPrintResets() -> bool
{
    MapStatistics stats;
    stats.set(Key::ActiveZones, 4);
    stats.increment(Key::TotalPacketsDelayedPerTick, 2);

    stats.print();

    return expectKnownZeroes(stats, "print reset known key value");
}

auto testMapStatisticsFlushResetsKnownKeysOnly() -> bool
{
    constexpr auto unknownKey = static_cast<Key>(9999);

    MapStatistics stats;
    stats.set(Key::ActiveMobs, 11);
    stats.set(unknownKey, 12);

    // With Tracy disabled for xi_test, flush still executes its report loop and
    // then resets the known enum keys. An arbitrary map key is not reset.
    stats.flush();

    return expectKnownZeroes(stats, "flush reset known key value") &&
           expectInt64(stats.get(unknownKey), 12, "flush preserves unknown key value");
}

} // namespace

auto runMapStatisticsSelfTests() -> bool
{
    bool ok = true;
    ok      = testMapStatisticsLabels() && ok;
    ok      = testMapStatisticsCounters() && ok;
    ok      = testMapStatisticsTimingAggregation() && ok;
    ok      = testMapStatisticsPrintResets() && ok;
    ok      = testMapStatisticsFlushResetsKnownKeysOnly() && ok;
    return ok;
}
