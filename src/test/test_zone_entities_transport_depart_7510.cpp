#include "test_zone_entities_transport_depart_7510.h"

#include "map/zone_entities_transport_depart.h"

#include <iostream>

auto runZoneEntitiesTransportDepart7510SelfTests() -> bool
{
    bool ok = true;
    if (zoneentities::PlanTransportDepartureRecovery(false, "scripts/zones/Boat/npcs/Captain.lua") != zoneentities::TransportDepartureRecoveryPlan{})
    {
        std::cerr << "zone transport depart 7510 self-test failed: no target skips\n";
        ok = false;
    }
    if (zoneentities::PlanTransportDepartureRecovery(true, "scripts/zones/Boat/npcs/Captain.lua") != zoneentities::TransportDepartureRecoveryPlan{ true, true, "scripts/zones/Boat/Zone.lua" })
    {
        std::cerr << "zone transport depart 7510 self-test failed: NPC script rewrites to zone\n";
        ok = false;
    }
    if (zoneentities::PlanTransportDepartureRecovery(true, "scripts/zones/Boat/Zone.lua") != zoneentities::TransportDepartureRecoveryPlan{ true, false, {} })
    {
        std::cerr << "zone transport depart 7510 self-test failed: clear target without rewrite\n";
        ok = false;
    }
    if (zoneentities::PlanTransportDepartureRecovery(true, "scripts/zones/Boat/.lua/npcs/Captain") != zoneentities::TransportDepartureRecoveryPlan{ true, true, "scripts/zones/Boat/.lua/Zone" })
    {
        std::cerr << "zone transport depart 7510 self-test failed: earlier suffix truncates NPC segment\n";
        ok = false;
    }
    return ok;
}
