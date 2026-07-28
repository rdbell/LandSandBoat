#include "test_zone_entities_mob_insert_7511.h"
#include "map/zone_entities_mob_insert.h"
#include <iostream>
auto runZoneEntitiesMobInsert7511SelfTests() -> bool
{
    bool ok = true;
    if (zoneentities::PlanMobInsertion(false, true) != zoneentities::MobInsertionPlan{}) { std::cerr << "zone mob insert 7511 self-test failed: non-mob skips\n"; ok = false; }
    if (zoneentities::PlanMobInsertion(true, true) != zoneentities::MobInsertionPlan{ true, zoneentities::MobInsertionList::Ally }) { std::cerr << "zone mob insert 7511 self-test failed: ally list\n"; ok = false; }
    if (zoneentities::PlanMobInsertion(true, false) != zoneentities::MobInsertionPlan{ true, zoneentities::MobInsertionList::Mob }) { std::cerr << "zone mob insert 7511 self-test failed: mob list\n"; ok = false; }
    return ok;
}
