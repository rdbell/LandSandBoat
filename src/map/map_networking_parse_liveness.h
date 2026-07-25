#pragma once

#include "map_networking_capacity.h"

// Pure liveness-admission seam for MapNetworking::parse before C2S frames are
// inspected.
namespace mapnetworkingparselivenesshelpers
{

struct Plan
{
    bool tapLastUpdate;
};

// MakePlan preserves parse's exclusion of pending-zone and waiting-for-zone
// traffic from the character liveness timestamp.
inline auto MakePlan(const bool pendingZone, const bool waitingForZone) -> Plan
{
    return Plan{
        .tapLastUpdate = mapnetworkinghelpers::ShouldTapLastUpdate(pendingZone, waitingForZone),
    };
}

} // namespace mapnetworkingparselivenesshelpers
