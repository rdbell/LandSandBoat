#include "test_zone_entities_summon_insert_7509.h"

#include "map/zone_entities_summon_insert.h"

#include <iostream>

auto runZoneEntitiesSummonInsert7509SelfTests() -> bool
{
    bool ok = true;
    if (zoneentities::PlanSummonInsertion(false, true) != zoneentities::SummonInsertionPlan{})
    {
        std::cerr << "zone summon insert 7509 self-test failed: null skips\n";
        ok = false;
    }
    if (zoneentities::PlanSummonInsertion(true, false) != zoneentities::SummonInsertionPlan{ true, zoneentities::DynamicTargidOwner::Zone, true })
    {
        std::cerr << "zone summon insert 7509 self-test failed: zone owner\n";
        ok = false;
    }
    if (zoneentities::PlanSummonInsertion(true, true) != zoneentities::SummonInsertionPlan{ true, zoneentities::DynamicTargidOwner::Instance, true })
    {
        std::cerr << "zone summon insert 7509 self-test failed: instance owner\n";
        ok = false;
    }
    return ok;
}
