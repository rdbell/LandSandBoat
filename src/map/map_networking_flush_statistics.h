#pragma once

#include <cstddef>
#include <span>

#include "map_networking_capacity.h"

// Pure aggregation seam for MapNetworking::flushStatistics.
namespace mapnetworkingflushstatisticshelpers
{

struct ZoneSnapshot
{
    bool        active;
    std::size_t playerCount;
    std::size_t mobCount;
    std::size_t dynamicTargIdCount;
};

struct Plan
{
    std::size_t activeZoneCount;
    std::size_t playerCount;
    std::size_t mobCount;
    int64       dynamicTargIdUsagePercent;
};

// Accumulator lets flushStatistics aggregate zone views without allocating an
// intermediate collection. Only active zones contribute to process-wide
// counts, and the dynamic TargID percentage uses the capacity derived from
// that same active-zone count.
class Accumulator
{
public:
    void add(const ZoneSnapshot& zone)
    {
        if (!zone.active)
        {
            return;
        }

        activeZoneCount_ += 1;
        playerCount_ += zone.playerCount;
        mobCount_ += zone.mobCount;
        dynamicTargIdCount_ += zone.dynamicTargIdCount;
    }

    auto plan() const -> Plan
    {
        const auto dynamicTargIdCapacity = mapnetworkinghelpers::AccumulateDynamicTargIdCapacity(activeZoneCount_);
        return Plan{
            .activeZoneCount           = activeZoneCount_,
            .playerCount               = playerCount_,
            .mobCount                  = mobCount_,
            .dynamicTargIdUsagePercent = mapnetworkinghelpers::DynamicTargIdUsagePercent(dynamicTargIdCount_, dynamicTargIdCapacity),
        };
    }

private:
    std::size_t activeZoneCount_    = 0;
    std::size_t playerCount_        = 0;
    std::size_t mobCount_           = 0;
    std::size_t dynamicTargIdCount_ = 0;
};

// MakePlan is the collection convenience wrapper around Accumulator, used by
// native tests to pin the aggregation policy.
inline auto MakePlan(const std::span<const ZoneSnapshot> zones) -> Plan
{
    Accumulator accumulator;
    for (const auto& zone : zones)
    {
        accumulator.add(zone);
    }
    return accumulator.plan();
}

} // namespace mapnetworkingflushstatisticshelpers
