#include "test_login_success_response_2767.h"

#include "login/login_success_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login success response 2767 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectReply(
    const loginHelpers::login_success_reply_plan& plan,
    const bool                                    wantTrustToken,
    const char* const                             label) -> bool
{
    return expect(plan.result == loginHelpers::LoginAttemptSuccessResult && plan.includeAccountId &&
                      plan.includeSessionHash && plan.includeTrustToken == wantTrustToken,
                  label);
}

} // namespace

auto runLoginSuccessResponse2767SelfTests() -> bool
{
    bool ok = true;

    ok = expect(loginHelpers::ShouldSyncSatchelOnOTP(true), "sync satchel when otpVerified") && ok;
    ok = expect(!loginHelpers::ShouldSyncSatchelOnOTP(false), "no satchel sync without otpVerified") && ok;

    ok = expectReply(loginHelpers::PlanLoginSuccessReply(false), false, "reply without trust token") && ok;
    ok = expectReply(loginHelpers::PlanLoginSuccessReply(true), true, "reply with trust token") && ok;

    // Composed PlanLoginSuccess: satchel + reply + ShouldIssueTrustToken
    {
        const auto plan = loginHelpers::PlanLoginSuccess(false, false);
        ok              = expect(!plan.syncSatchel, "compose: no otp no satchel") && ok;
        ok              = expectReply(plan.reply, false, "compose: no otp no trust token") && ok;
    }
    {
        const auto plan = loginHelpers::PlanLoginSuccess(false, true);
        ok              = expect(!plan.syncSatchel, "compose: trust flag alone no satchel") && ok;
        ok              = expectReply(plan.reply, false, "compose: trust flag alone no token") && ok;
    }
    {
        const auto plan = loginHelpers::PlanLoginSuccess(true, false);
        ok              = expect(plan.syncSatchel, "compose: otpVerified syncs satchel") && ok;
        ok              = expectReply(plan.reply, false, "compose: otp without trust_this_computer") && ok;
    }
    {
        const auto plan = loginHelpers::PlanLoginSuccess(true, true);
        ok              = expect(plan.syncSatchel, "compose: both sync satchel") && ok;
        ok              = expectReply(plan.reply, true, "compose: both issue trust token") && ok;
    }

    // ShouldIssueTrustToken remains the underlying gate (not reimplemented).
    ok = expect(loginHelpers::ShouldIssueTrustToken(true, true), "ShouldIssueTrustToken both") && ok;
    ok = expect(!loginHelpers::ShouldIssueTrustToken(true, false), "ShouldIssueTrustToken no otp") && ok;
    ok = expect(!loginHelpers::ShouldIssueTrustToken(false, true), "ShouldIssueTrustToken no flag") && ok;
    ok = expect(!loginHelpers::ShouldIssueTrustToken(false, false), "ShouldIssueTrustToken neither") && ok;

    return ok;
}
