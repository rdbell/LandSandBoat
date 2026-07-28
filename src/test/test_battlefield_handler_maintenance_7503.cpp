#include "test_battlefield_handler_maintenance_7503.h"

#include "map/battlefield_handler_maintenance.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield handler maintenance 7503 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattlefieldHandlerMaintenance7503SelfTests() -> bool
{
    using battlefieldhandlerhelpers::HasReachedMaxCapacity;
    using battlefieldhandlerhelpers::OrphanedPlayerPlan;
    using battlefieldhandlerhelpers::PlanOrphanedPlayer;

    bool ok = true;
    ok = expect(!HasReachedMaxCapacity(2, 3), "below capacity admits") && ok;
    ok = expect(HasReachedMaxCapacity(3, 3), "at capacity rejects") && ok;
    ok = expect(HasReachedMaxCapacity(4, 3), "over capacity rejects") && ok;
    ok = expect(HasReachedMaxCapacity(0, 0), "zero capacity is full") && ok;
    ok = expect(PlanOrphanedPlayer(true, false) == OrphanedPlayerPlan{ true, false, false }, "pre-expiry keeps entry") && ok;
    ok = expect(PlanOrphanedPlayer(false, false) == OrphanedPlayerPlan{ false, false, true }, "expired missing character removes") && ok;
    ok = expect(PlanOrphanedPlayer(false, true) == OrphanedPlayerPlan{ false, true, true }, "expired present character kicks and removes") && ok;
    return ok;
}
