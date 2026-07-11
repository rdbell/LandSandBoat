#include "test_login_auth_success_reply_1315.h"

#include "login/auth_password.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login auth success reply 1315 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginAuthSuccessReply1315SelfTests() -> bool
{
    bool ok = true;

    ok = expect(loginHelpers::SessionHashSeed(0, 0) == 0, "seed zero") && ok;
    ok = expect(loginHelpers::SessionHashSeed(0xA5A5A5A5, 0x5A5A5A5A) == 0xFFFFFFFFu, "seed xor all bits") && ok;
    ok = expect(loginHelpers::SessionHashSeed(100, 3) == 103, "seed 100^3") && ok;
    ok = expect(loginHelpers::SessionHashSeed(std::numeric_limits<uint32>::max(), 1) == 0xFFFFFFFEu, "seed max^1") && ok;
    ok = expect(loginHelpers::SessionHashLength == 16, "session hash length") && ok;

    ok = expect(loginHelpers::LoginAttemptSuccessResult == login_result::LOGIN_SUCCESS, "login success result") && ok;
    ok = expect(loginHelpers::ChangePasswordSuccessResult == login_result::LOGIN_SUCCESS_CHANGE_PASSWORD,
                "change password success result") &&
         ok;
    ok = expect(loginHelpers::ChangePasswordSuccessAccountID == 0, "change password account_id") && ok;
    ok = expect(std::string(loginHelpers::ChangePasswordSuccessSessionHash) == "", "change password empty session_hash") && ok;

    ok = expect(loginHelpers::FormatUnhandledAuthCode(0, "1.2.3.4") == "Unhandled auth code: 0 from 1.2.3.4",
                "unhandled zero code") &&
         ok;
    ok = expect(loginHelpers::FormatUnhandledAuthCode(static_cast<int8>(0x10), "127.0.0.1") ==
                    "Unhandled auth code: 16 from 127.0.0.1",
                "unhandled LOGIN_ATTEMPT code as unknown path still formats") &&
         ok;
    ok = expect(loginHelpers::FormatUnhandledAuthCode(static_cast<int8>(-1), "10.0.0.1") ==
                    "Unhandled auth code: -1 from 10.0.0.1",
                "signed int8 negative boundary") &&
         ok;
    ok = expect(loginHelpers::FormatUnhandledAuthCode(std::numeric_limits<int8>::max(), "host") ==
                    "Unhandled auth code: 127 from host",
                "signed int8 max") &&
         ok;

    return ok;
}
