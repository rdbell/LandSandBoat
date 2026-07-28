#include "test_battlefield_handler_registration_7501.h"

#include "map/battlefield_handler_registration.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield handler registration 7501 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runBattlefieldHandlerRegistration7501SelfTests() -> bool
{
    using battlefieldhandlerhelpers::PlanRegistration;
    using battlefieldhandlerhelpers::RegistrationAction;
    using battlefieldhandlerhelpers::RegistrationPlan;

    bool ok = true;
    const struct
    {
        bool             alreadyInBattlefield;
        bool             hasExistingBattlefield;
        bool             hasBattlefieldStatus;
        bool             enterExistingRequested;
        bool             existingBattlefieldInProgress;
        bool             registrationAreaMatches;
        RegistrationPlan want;
        const char*      label;
    } cases[] = {
        { true, false, false, false, false, false, {}, "already in a battlefield waits" },
        { false, false, true, false, false, false, { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_REQS_NOT_MET, false }, "orphan status rejects" },
        { false, false, false, true, false, false, { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_REQS_NOT_MET, true }, "stale enter-existing resets and rejects" },
        { false, false, false, false, false, false, { RegistrationAction::LoadNew, BATTLEFIELD_RETURN_CODE_WAIT, false }, "unregistered player loads new" },
        { false, true, false, false, false, true, { RegistrationAction::LoadNew, BATTLEFIELD_RETURN_CODE_WAIT, false }, "existing registration without status loads new" },
        { false, true, true, false, true, true, { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_LOCKED, true }, "in-progress battlefield locks" },
        { false, true, true, false, false, false, { RegistrationAction::Return, BATTLEFIELD_RETURN_CODE_INCREMENT_REQUEST, true }, "other area requests increment" },
        { false, true, true, false, false, true, { RegistrationAction::InsertExisting, BATTLEFIELD_RETURN_CODE_WAIT, true }, "matching open battlefield inserts" },
    };

    for (const auto& c : cases)
    {
        const auto got = PlanRegistration(c.alreadyInBattlefield, c.hasExistingBattlefield, c.hasBattlefieldStatus,
                                          c.enterExistingRequested, c.existingBattlefieldInProgress, c.registrationAreaMatches);
        ok             = expect(got == c.want, c.label) && ok;
    }

    return ok;
}
