#include "test_zone_in_cleanup_7517.h"

#include "map/zone_in_cleanup.h"

#include <iostream>

auto runZoneInCleanup7517SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "zone-in cleanup 7517 self-test failed: " << label << '\n';
        }
        return value;
    };

    using zonehelpers::PlanZoneInCleanup;
    using zonehelpers::ZoneInCleanupPlan;

    return expect(PlanZoneInCleanup(false, false) == ZoneInCleanupPlan{ false, true }, "ordinary zone cleanup") &&
           expect(PlanZoneInCleanup(true, false) == ZoneInCleanupPlan{ true, true }, "illusion ordinary zone") &&
           expect(PlanZoneInCleanup(false, true) == ZoneInCleanupPlan{ false, false }, "instanced zone retains state") &&
           expect(PlanZoneInCleanup(true, true) == ZoneInCleanupPlan{ true, false }, "illusion clears in instance");
}
