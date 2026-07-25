#pragma once

#include <cstddef>

// Pure debug-backlog seam for MapNetworking::send_parse after a packet send.
namespace mapnetworkingsendbackloghelpers
{

struct Plan
{
    bool reportRemainingPackets;
    bool warnUnzonedBacklog;
    bool warnZonedBacklog;
    bool clearPacketQueue;
};

// MakePlan preserves send_parse's debug-only, strict backlog threshold:
// unzoned queues are warned and cleared; zoned queues are only warned.
inline auto MakePlan(
    const bool debugBacklogEnabled,
    const std::size_t remainingPackets,
    const bool characterHasZone,
    const std::size_t maxPacketBacklogSize) -> Plan
{
    if (!debugBacklogEnabled)
    {
        return Plan{};
    }
    Plan plan{
        .reportRemainingPackets = true,
    };
    if (remainingPackets <= maxPacketBacklogSize)
    {
        return plan;
    }
    if (!characterHasZone)
    {
        return Plan{
            .reportRemainingPackets = true,
            .warnUnzonedBacklog = true,
            .warnZonedBacklog   = false,
            .clearPacketQueue   = true,
        };
    }
    return Plan{
        .reportRemainingPackets = true,
        .warnUnzonedBacklog = false,
        .warnZonedBacklog   = true,
        .clearPacketQueue   = false,
    };
}

} // namespace mapnetworkingsendbackloghelpers
