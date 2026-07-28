#include "test_zone_in_treasure_pool_7516.h"

#include "map/zone_in_treasure_pool.h"

#include <iostream>

auto runZoneInTreasurePool7516SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "zone-in treasure pool 7516 self-test failed: " << label << '\n';
        }
        return value;
    };

    using zonehelpers::PlanZoneInTreasurePool;
    using zonehelpers::ZoneInTreasurePoolAction;

    return expect(PlanZoneInTreasurePool(true, true) == ZoneInTreasurePoolAction::AttachZonePool, "zone pool overrides party") &&
           expect(PlanZoneInTreasurePool(true, false) == ZoneInTreasurePoolAction::AttachZonePool, "zone pool attaches") &&
           expect(PlanZoneInTreasurePool(false, true) == ZoneInTreasurePoolAction::ReloadPartyPool, "party reload") &&
           expect(PlanZoneInTreasurePool(false, false) == ZoneInTreasurePoolAction::CreateSoloPool, "solo creation");
}
