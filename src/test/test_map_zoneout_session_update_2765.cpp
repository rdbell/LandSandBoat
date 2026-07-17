#include "test_map_zoneout_session_update_2765.h"

#include "map/map_networking_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map zoneout session update 2765 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapZoneOutSessionUpdate2765SelfTests() -> bool
{
    const auto zoneTransition = mapnetworkinghelpers::PlanZoneOutSessionUpdate(false);
    const auto fullLogout     = mapnetworkinghelpers::PlanZoneOutSessionUpdate(true);

    return expect(zoneTransition.updateServerEndpoint && zoneTransition.clearClientPort && zoneTransition.stampLastZoneoutTime, "zone transition") &&
           expect(!fullLogout.updateServerEndpoint && fullLogout.clearClientPort && fullLogout.stampLastZoneoutTime, "full logout");
}
