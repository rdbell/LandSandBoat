#include "test_map_networking_zone_packet_2717.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingZonePacket2717SelfTests() -> bool
{
    using mapnetworkinghelpers::IncomingPacketZonePlan;
    using mapnetworkinghelpers::PlanIncomingPacketForZone;

    const bool ok = PlanIncomingPacketForZone(true, false, false) == IncomingPacketZonePlan::Dispatch &&
                    PlanIncomingPacketForZone(false, true, false) == IncomingPacketZonePlan::Dispatch &&
                    PlanIncomingPacketForZone(false, true, true) == IncomingPacketZonePlan::Dispatch &&
                    PlanIncomingPacketForZone(false, false, true) == IncomingPacketZonePlan::SkipUnexpectedPendingZone &&
                    PlanIncomingPacketForZone(false, false, false) == IncomingPacketZonePlan::WarnAndSkipUnexpected;
    if (!ok)
    {
        std::cerr << "map networking zone packet 2717 self-test failed\n";
    }
    return ok;
}
