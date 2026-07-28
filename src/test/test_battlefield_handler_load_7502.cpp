#include "test_battlefield_handler_load_7502.h"

#include "map/battlefield_handler_load.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield handler load 7502 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runBattlefieldHandlerLoad7502SelfTests() -> bool
{
    using battlefieldhandlerhelpers::LoadAction;
    using battlefieldhandlerhelpers::LoadAdmissionPlan;
    using battlefieldhandlerhelpers::PlanLoadAdmission;
    using battlefieldhandlerhelpers::PlanLoadRecordResult;

    bool ok = true;
    const struct
    {
        bool              alreadyInBattlefield;
        bool              atBattlefieldCapacity;
        bool              registrationAreaOccupied;
        bool              cutsceneOnlyRegistration;
        LoadAdmissionPlan want;
        const char*       label;
    } cases[] = {
        { true, false, false, false, {}, "assigned character waits" },
        { false, true, false, false, {}, "capacity waits" },
        { false, false, true, false, { LoadAction::Return, BATTLEFIELD_RETURN_CODE_INCREMENT_REQUEST }, "occupied area increments" },
        { false, false, false, true, { LoadAction::Return, BATTLEFIELD_RETURN_CODE_CUTSCENE }, "cutscene-only registration returns cutscene" },
        { false, false, false, false, { LoadAction::Create, BATTLEFIELD_RETURN_CODE_WAIT }, "free ordinary registration creates" },
    };

    for (const auto& c : cases)
    {
        ok = expect(PlanLoadAdmission(c.alreadyInBattlefield, c.atBattlefieldCapacity, c.registrationAreaOccupied,
                                      c.cutsceneOnlyRegistration) == c.want,
                    c.label) &&
             ok;
    }

    ok = expect(PlanLoadRecordResult(false) == BATTLEFIELD_RETURN_CODE_REQS_NOT_MET, "missing record rejects") && ok;
    ok = expect(PlanLoadRecordResult(true) == BATTLEFIELD_RETURN_CODE_CUTSCENE, "record creates cutscene") && ok;
    return ok;
}
