#include "test_login_auth_login_attempt_otp_1314.h"

#include "login/auth_password.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth login-attempt OTP 1314 self-test failed: " << label << '\n';
    }
    return condition;
}

using outcome = loginHelpers::login_attempt_otp_outcome;

auto expectPlan(
    const bool        needsOTP,
    const std::string& trustToken,
    const std::string& otp,
    const bool        trustTokenValid,
    const bool        totpValid,
    const outcome     wantOutcome,
    const bool        wantVerified,
    const char*       label) -> bool
{
    const auto plan = loginHelpers::PlanLoginAttemptOTP(needsOTP, trustToken, otp, trustTokenValid, totpValid);
    return expect(plan.outcome == wantOutcome && plan.otpVerified == wantVerified, label);
}

} // namespace

auto runLoginAuthLoginAttemptOTP1314SelfTests() -> bool
{
    bool ok = true;

    ok = expectPlan(false, "", "", false, false, outcome::SKIP, false, "no OTP needed skips") && ok;
    ok = expectPlan(false, "token", "123456", true, true, outcome::SKIP, false, "no OTP ignores trust/otp inputs") && ok;

    ok = expectPlan(true, "token", "", true, false, outcome::VERIFIED, true, "valid trust verifies without OTP") && ok;
    ok = expectPlan(true, "token", "123456", true, false, outcome::VERIFIED, true, "valid trust ignores totpValid") && ok;

    ok = expectPlan(true, "token", "", false, false, outcome::REJECT_TRUST_TOKEN, false, "invalid trust no OTP") && ok;
    ok = expectPlan(true, "token", "123456", false, true, outcome::VERIFIED, true, "invalid trust falls back to OTP") && ok;
    ok = expectPlan(true, "token", "123456", false, false, outcome::REJECT_OTP, false, "invalid trust bad OTP") && ok;

    ok = expectPlan(true, "", "123456", false, true, outcome::VERIFIED, true, "empty trust good OTP") && ok;
    ok = expectPlan(true, "", "123456", false, false, outcome::REJECT_OTP, false, "empty trust bad OTP") && ok;
    ok = expectPlan(true, "", "", false, false, outcome::REJECT_OTP, false, "empty trust empty OTP") && ok;

    // Result mapping
    ok = expect(!loginHelpers::LoginResultForLoginAttemptOTP(outcome::SKIP).has_value(), "SKIP no result") && ok;
    ok = expect(!loginHelpers::LoginResultForLoginAttemptOTP(outcome::VERIFIED).has_value(), "VERIFIED no result") && ok;

    const auto trustReject = loginHelpers::LoginResultForLoginAttemptOTP(outcome::REJECT_TRUST_TOKEN);
    ok = expect(trustReject.has_value() && *trustReject == login_result::LOGIN_ERROR_TRUST_TOKEN_INVALID,
                "TRUST_TOKEN maps") &&
         ok;

    const auto otpReject = loginHelpers::LoginResultForLoginAttemptOTP(outcome::REJECT_OTP);
    ok = expect(otpReject.has_value() && *otpReject == login_result::LOGIN_ERROR, "OTP reject maps") && ok;

    ok = expect(loginHelpers::ShouldIssueTrustToken(true, true), "issue when both") && ok;
    ok = expect(!loginHelpers::ShouldIssueTrustToken(true, false), "no issue without otpVerified") && ok;
    ok = expect(!loginHelpers::ShouldIssueTrustToken(false, true), "no issue without trustThisComputer") && ok;
    ok = expect(!loginHelpers::ShouldIssueTrustToken(false, false), "no issue when neither") && ok;

    return ok;
}
