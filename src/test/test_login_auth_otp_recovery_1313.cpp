#include "test_login_auth_otp_recovery_1313.h"

#include "login/auth_password.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth OTP recovery 1313 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginAuthOTPRecovery1313SelfTests() -> bool
{
    bool ok = true;

    ok = expect(loginHelpers::MatchesRecoveryCode("ABC123", "ABC123"), "exact match") && ok;
    ok = expect(loginHelpers::MatchesRecoveryCode("abc123", "ABC123"), "case-insensitive lower otp") && ok;
    ok = expect(loginHelpers::MatchesRecoveryCode("AbC123", "aBc123"), "mixed case both sides") && ok;
    ok = expect(!loginHelpers::MatchesRecoveryCode("ABC123", "ABC124"), "different codes") && ok;
    ok = expect(!loginHelpers::MatchesRecoveryCode("ABC12", "ABC123"), "prefix not equal") && ok;
    ok = expect(!loginHelpers::MatchesRecoveryCode("ABC123", "ABC12"), "suffix not equal") && ok;
    ok = expect(loginHelpers::MatchesRecoveryCode("", ""), "empty both") && ok;
    ok = expect(!loginHelpers::MatchesRecoveryCode("x", ""), "empty recovery only") && ok;
    ok = expect(!loginHelpers::MatchesRecoveryCode("", "x"), "empty otp only") && ok;

    ok = expect(loginHelpers::AcceptsOTPOrRecovery(true, "wrong", "CODE"), "totpValid accepts without recovery") && ok;
    ok = expect(loginHelpers::AcceptsOTPOrRecovery(false, "CODE", "code"), "recovery accepts when totp fails") && ok;
    ok = expect(loginHelpers::AcceptsOTPOrRecovery(true, "CODE", "code"), "both valid") && ok;
    ok = expect(!loginHelpers::AcceptsOTPOrRecovery(false, "wrong", "CODE"), "neither accepts") && ok;
    ok = expect(!loginHelpers::AcceptsOTPOrRecovery(false, "", "CODE"), "empty otp fails recovery unless match") && ok;
    ok = expect(loginHelpers::AcceptsOTPOrRecovery(false, "", ""), "empty recovery matches empty otp") && ok;

    return ok;
}
