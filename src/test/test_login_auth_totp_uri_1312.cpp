#include "test_login_auth_totp_uri_1312.h"

#include "login/auth_password.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth TOTP URI 1312 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginAuthTOTPURI1312SelfTests() -> bool
{
    bool ok = true;

    ok = expect(loginHelpers::TOTPURIDigits == 6, "digits constant") && ok;
    ok = expect(loginHelpers::TOTPURIPeriod == 30, "period constant") && ok;
    ok = expect(std::string(loginHelpers::TOTPURIAlgorithm) == "SHA1", "algorithm constant") && ok;
    ok = expect(std::string(loginHelpers::FailedCredentialValidationMessage) == "Failed to validate credentials",
                "credential failure message") &&
         ok;

    const auto canonical = loginHelpers::FormatTOTPURI("OmegaXI", "Alice", "JBSWY3DPEHPK3PXP");
    ok = expect(canonical ==
                    "otpauth://totp/OmegaXI:Alice?secret=JBSWY3DPEHPK3PXP&issuer=OmegaXI&algorithm=SHA1&digits=6&period=30",
                "canonical URI") &&
         ok;

    // No URL encoding — special characters pass through as raw text.
    const auto special = loginHelpers::FormatTOTPURI("Srv Name", "user@host", "sec+ret/1");
    ok = expect(special ==
                    "otpauth://totp/Srv Name:user@host?secret=sec+ret/1&issuer=Srv Name&algorithm=SHA1&digits=6&period=30",
                "unescaped special characters") &&
         ok;

    // Empty server/user still interpolate (LSB does not gate on them here).
    const auto emptyNames = loginHelpers::FormatTOTPURI("", "", "ABC");
    ok = expect(emptyNames == "otpauth://totp/:?secret=ABC&issuer=&algorithm=SHA1&digits=6&period=30",
                "empty server and user") &&
         ok;

    ok = expect(loginHelpers::IsCreateTOTPSecretValid("ABC"), "non-empty secret valid") && ok;
    ok = expect(!loginHelpers::IsCreateTOTPSecretValid(""), "empty secret invalid") && ok;
    ok = expect(loginHelpers::IsCreateTOTPSecretValid(" "), "whitespace secret valid") && ok;

    return ok;
}
