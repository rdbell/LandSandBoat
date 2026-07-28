#include "test_zone_entities_npc_insert_7508.h"

#include "map/zone_entities_npc_insert.h"

#include <iostream>

auto runZoneEntitiesNpcInsert7508SelfTests() -> bool
{
    bool ok = true;
    if (zoneentities::PlanNpcInsertion(false, true, true) != zoneentities::NpcInsertionPlan{})
    {
        std::cerr << "zone NPC insert 7508 self-test failed: non-NPC skips\n";
        ok = false;
    }
    if (zoneentities::PlanNpcInsertion(true, false, false) != zoneentities::NpcInsertionPlan{ true, false, false })
    {
        std::cerr << "zone NPC insert 7508 self-test failed: ordinary NPC list\n";
        ok = false;
    }
    if (zoneentities::PlanNpcInsertion(true, true, true) != zoneentities::NpcInsertionPlan{ true, true, true })
    {
        std::cerr << "zone NPC insert 7508 self-test failed: ship duplicate\n";
        ok = false;
    }
    return ok;
}
