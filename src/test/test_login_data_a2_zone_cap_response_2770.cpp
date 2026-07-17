#include "test_login_data_a2_zone_cap_response_2770.h"

#include "login/data_a2.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login data A2 zone cap response 2770 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(
    const loginHelpers::data_a2_zone_cap_response_plan& plan,
    const bool                                          logWarning,
    const bool                                          writeWorldFullError,
    const bool                                          returnFromRead,
    const char* const                                   label) -> bool
{
    return expect(plan.logWarning == logWarning &&
                      plan.writeWorldFullError == writeWorldFullError &&
                      plan.returnFromRead == returnFromRead,
                  label);
}

} // namespace

auto runLoginDataA2ZoneCapResponse2770SelfTests() -> bool
{
    bool ok = true;

    ok = expectPlan(loginHelpers::PlanDataA2ZoneCapResponse(false, false), false, false, false, "not at cap, no view") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2ZoneCapResponse(false, true), false, false, false, "not at cap, has view") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2ZoneCapResponse(true, true), true, true, true, "at cap with view: warn+error+return") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2ZoneCapResponse(true, false), true, false, false, "at cap without view: warn only") && ok;

    return ok;
}
