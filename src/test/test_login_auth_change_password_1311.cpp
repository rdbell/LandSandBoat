#include "test_login_auth_change_password_1311.h"

#include "login/auth_password.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth change password 1311 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(const uint32 status, const bool wantBanned, const bool wantUpdate, const char* label) -> bool
{
    const auto plan = loginHelpers::PlanChangePasswordStatus(status);
    return expect(plan.emitBannedError == wantBanned && plan.attemptUpdate == wantUpdate, label);
}

} // namespace

auto runLoginAuthChangePassword1311SelfTests() -> bool
{
    bool ok = true;

    ok = expectPlan(0, false, false, "zero status neither banned nor update") && ok;
    ok = expectPlan(ACCOUNT_STATUS_CODE::NORMAL, false, true, "NORMAL updates without banned emit") && ok;
    ok = expectPlan(ACCOUNT_STATUS_CODE::BANNED, true, false, "BANNED emits without update") && ok;
    ok = expectPlan(static_cast<uint32>(ACCOUNT_STATUS_CODE::NORMAL) | static_cast<uint32>(ACCOUNT_STATUS_CODE::BANNED),
                    true,
                    true,
                    "NORMAL|BANNED emits then still attempts update") &&
         ok;
    ok = expectPlan(0x04, false, false, "unknown flag neither banned nor update") && ok;

    ok = expect(loginHelpers::IsEmptyUpdatedPassword(""), "empty string is empty") && ok;
    ok = expect(loginHelpers::IsEmptyUpdatedPassword(std::string{}), "default string is empty") && ok;
    ok = expect(!loginHelpers::IsEmptyUpdatedPassword("x"), "non-empty not empty") && ok;
    ok = expect(!loginHelpers::IsEmptyUpdatedPassword(" "), "whitespace is not empty") && ok;

    return ok;
}
