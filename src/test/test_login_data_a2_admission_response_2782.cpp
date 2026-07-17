#include "test_login_data_a2_admission_response_2782.h"

#include "login/data_a2.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login data A2 admission response 2782 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(
    const loginHelpers::data_a2_admission_response_plan& plan,
    const bool                                           proceedWithLogin,
    const bool                                           writeLobbyError,
    const bool                                           returnFromRead,
    const char* const                                    label) -> bool
{
    return expect(plan.proceedWithLogin == proceedWithLogin &&
                      plan.writeLobbyError == writeLobbyError &&
                      plan.returnFromRead == returnFromRead,
                  label);
}

} // namespace

auto runLoginDataA2AdmissionResponse2782SelfTests() -> bool
{
    using decision = loginHelpers::data_a2_admission_decision;

    bool ok = true;

    ok = expectPlan(loginHelpers::PlanDataA2AdmissionResponse(decision::ALLOWED, true), true, false, false, "allowed with view") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2AdmissionResponse(decision::ALLOWED, false), true, false, false, "allowed without view") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2AdmissionResponse(decision::LOBBY_DENIED, true), false, true, true, "lobby denied with view: error+return") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2AdmissionResponse(decision::LOBBY_DENIED, false), false, false, false, "lobby denied without view: fall through") && ok;
    // ZONE_AT_CAP is not expected after the zone-cap path (Decide uses zoneAtCap=false),
    // but non-ALLOWED still follows the LOBBY_DENIED host branch.
    ok = expectPlan(loginHelpers::PlanDataA2AdmissionResponse(decision::ZONE_AT_CAP, true), false, true, true, "zone at cap with view treated as deny") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2AdmissionResponse(decision::ZONE_AT_CAP, false), false, false, false, "zone at cap without view: fall through") && ok;

    return ok;
}
