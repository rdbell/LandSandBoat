#include "test_zone_seal_restore_2661.h"
#include "map/zone_capacity.h"

auto runZoneSealRestore2661SelfTests() -> bool
{
    return !zonehelpers::PlanSealTimerRestore(true, 100, 100).restore && zonehelpers::PlanSealTimerRestore(true, 400, 100).restore && !zonehelpers::PlanSealTimerRestore(true, 401, 100).restore;
}
