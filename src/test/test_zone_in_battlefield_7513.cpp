#include "test_zone_in_battlefield_7513.h"

#include "map/zone_in_battlefield.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone-in battlefield 7513 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runZoneInBattlefield7513SelfTests() -> bool
{
    using zonehelpers::PlanZoneInBattlefield;
    using zonehelpers::ZoneInBattlefieldAction;
    using zonehelpers::ZoneInBattlefieldPlan;

    bool ok = true;
    ok = expect(PlanZoneInBattlefield(true, true, true, false, true, true, false) ==
                    ZoneInBattlefieldPlan{ ZoneInBattlefieldAction::InsertRegistered, false, false, false },
                "registered confrontation re-enters") && ok;
    ok = expect(PlanZoneInBattlefield(true, true, true, true, false, false, true) ==
                    ZoneInBattlefieldPlan{ ZoneInBattlefieldAction::InsertRegistered, true, false, false },
                "registered entry state forwards") && ok;
    ok = expect(PlanZoneInBattlefield(true, false, true, true, true, false, false) ==
                    ZoneInBattlefieldPlan{ ZoneInBattlefieldAction::AddOrphaned, false, false, false },
                "unregistered arena player becomes orphan") && ok;
    ok = expect(PlanZoneInBattlefield(true, false, true, false, true, false, false) ==
                    ZoneInBattlefieldPlan{ ZoneInBattlefieldAction::ClearConfrontation, false, true, true },
                "unregistered exterior player clears confrontation and restriction") && ok;
    ok = expect(PlanZoneInBattlefield(false, false, true, false, true, false, false) ==
                    ZoneInBattlefieldPlan{ ZoneInBattlefieldAction::ClearConfrontation, false, true, false },
                "no handler clears confrontation without restriction update") && ok;
    ok = expect(PlanZoneInBattlefield(false, false, false, false, false, true, false) ==
                    ZoneInBattlefieldPlan{ ZoneInBattlefieldAction::ClearLevelSync, false, false, false },
                "handlerless solo level sync clears") && ok;
    ok = expect(PlanZoneInBattlefield(true, false, false, false, false, true, false) == ZoneInBattlefieldPlan{},
                "handler presence suppresses solo level sync cleanup") && ok;
    return ok;
}
