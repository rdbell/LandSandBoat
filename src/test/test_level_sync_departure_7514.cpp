#include "test_level_sync_departure_7514.h"

#include "map/level_sync_departure.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "level-sync departure 7514 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runLevelSyncDeparture7514SelfTests() -> bool
{
    using partyhelpers::LevelSyncDepartureAction;
    using partyhelpers::LevelSyncDeparturePlan;
    using partyhelpers::PlanLevelSyncDeparture;

    bool ok = true;
    ok = expect(PlanLevelSyncDeparture(false, true, true, true, 0) == LevelSyncDeparturePlan{}, "unrestricted skips all work") && ok;
    ok = expect(PlanLevelSyncDeparture(true, false, false, false, 0) ==
                    LevelSyncDeparturePlan{ LevelSyncDepartureAction::None, true },
                "restricted solo clears local effects") && ok;
    ok = expect(PlanLevelSyncDeparture(true, true, true, true, 0) ==
                    LevelSyncDeparturePlan{ LevelSyncDepartureAction::DeactivateLeftArea, true },
                "designee or leader deactivates first") && ok;
    ok = expect(PlanLevelSyncDeparture(true, true, false, true, 1) ==
                    LevelSyncDeparturePlan{ LevelSyncDepartureAction::RemoveTooFewMembers, true },
                "one remaining peer removes sync") && ok;
    ok = expect(PlanLevelSyncDeparture(true, true, false, true, 2) ==
                    LevelSyncDeparturePlan{ LevelSyncDepartureAction::None, true },
                "two remaining peers retain sync") && ok;
    return ok;
}
