#include "test_login_data_a2_already_logged_in_2776.h"

#include "login/data_a2.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login data A2 already logged in 2776 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(
    const loginHelpers::data_a2_already_logged_in_plan& plan,
    const bool                                          incrementKey,
    const bool                                          writeAlreadyLoggedInError,
    const bool                                          returnFromRead,
    const char* const                                   label) -> bool
{
    return expect(plan.incrementKey == incrementKey &&
                      plan.writeAlreadyLoggedInError == writeAlreadyLoggedInError &&
                      plan.returnFromRead == returnFromRead,
                  label);
}

} // namespace

auto runLoginDataA2AlreadyLoggedIn2776SelfTests() -> bool
{
    bool ok = true;

    ok = expect(loginHelpers::DataA2AlreadyLoggedInKeyIncrement == 1, "key increment constant") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2AlreadyLoggedInResponse(true), true, true, true, "has view: all true") && ok;
    ok = expectPlan(loginHelpers::PlanDataA2AlreadyLoggedInResponse(false), false, false, false, "no view: fall through") && ok;

    return ok;
}
