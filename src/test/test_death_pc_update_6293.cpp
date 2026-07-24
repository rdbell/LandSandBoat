#include "test_death_pc_update_6293.h"

#include "map/ai/states/death_pc_update.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "death pc update 6293 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runDeathPCUpdate6293SelfTests() -> bool
{
    using deathpcupdate::shouldAutoHomepoint;
    using deathpcupdate::shouldExitAfterRaise;
    using deathpcupdate::shouldOfferRaiseMenu;

    return expect(shouldExitAfterRaise(true, true, true), "accepted completed past+2s exits") &&
           expect(!shouldExitAfterRaise(true, true, false), "accepted completed at boundary stays") &&
           expect(!shouldExitAfterRaise(false, true, true), "not accepted does not exit") &&
           expect(!shouldExitAfterRaise(true, false, true), "not completed does not exit") &&
           expect(!shouldExitAfterRaise(false, false, true), "neither accepted nor completed stays") &&
           expect(shouldAutoHomepoint(true), "past death deadline auto-homepoints") &&
           expect(!shouldAutoHomepoint(false), "before death deadline does not auto-homepoint") &&
           expect(shouldOfferRaiseMenu(true, false, true, true), "dead hasRaise after raiseTime offers") &&
           expect(!shouldOfferRaiseMenu(false, false, true, true), "at raiseTime does not offer") &&
           expect(!shouldOfferRaiseMenu(true, true, true, true), "already sent does not offer") &&
           expect(!shouldOfferRaiseMenu(true, false, false, true), "not dead does not offer") &&
           expect(!shouldOfferRaiseMenu(true, false, true, false), "no raise does not offer");
}
