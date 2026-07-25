#pragma once

#include <common/cbasetypes.h>

#include "map_networking_capacity.h"

// Pure parse-tail seam for MapNetworking::parse after the small-packet loop.
namespace mapnetworkingparsetailhelpers
{

struct Plan
{
    uint16                                    clientPacketID;
    mapnetworkinghelpers::AcknowledgementPlan acknowledgement;
};

// MakePlan stores the enclosing datagram code as the latest client packet ID,
// then selects the outgoing acknowledgement action using the final small
// packet type, exactly as MapNetworking::parse does.
inline auto MakePlan(
    const uint16 datagramCode,
    const uint16 acknowledgedServerPacketID,
    const uint16 serverPacketID,
    const uint16 lastSmallPacketType) -> Plan
{
    return Plan{
        .clientPacketID = datagramCode,
        .acknowledgement = mapnetworkinghelpers::PlanOutgoingPacketAcknowledgement(
            acknowledgedServerPacketID,
            serverPacketID,
            lastSmallPacketType),
    };
}

} // namespace mapnetworkingparsetailhelpers
