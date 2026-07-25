#include "test_map_networking_incoming_packet_plan_6914.h"

#include "map/map_networking_incoming_packet_plan.h"

#include <iostream>

auto runMapNetworkingIncomingPacketPlan6914SelfTests() -> bool
{
    using mapnetworkingincominghelpers::Facts;
    using mapnetworkingincominghelpers::MakePlan;
    using mapnetworkingincominghelpers::Plan;

    const bool ok =
        MakePlan({ false, 0, false, false, true, false }) == Plan{ true, false, false, false, false, false, false } &&
        MakePlan({ true, 0, true, false, false, false }) == Plan{ false, true, true, false, true, true, false } &&
        MakePlan({ true, 1, false, true, true, false }) == Plan{ false, false, false, true, true, true, false } &&
        MakePlan({ true, -1, false, false, true, true }) == Plan{ false, false, false, false, false, false, true };

    if (!ok)
    {
        std::cerr << "map networking incoming packet plan 6914 self-test failed\n";
    }

    return ok;
}
