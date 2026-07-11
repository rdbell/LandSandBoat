#include "test_login_auth_password_1307.h"

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
        std::cerr << "login auth password 1307 self-test failed: " << label << '\n';
    }
    return condition;
}

// Build a 60-byte Modular Crypt Format bcrypt string with the given version
// letter and filler body so length and prefix are exact.
auto makeBcrypt(const char version, const char fill = 'A') -> std::string
{
    std::string hash = "$2";
    hash.push_back(version);
    hash.push_back('$');
    hash.append(56, fill);
    return hash;
}

} // namespace

auto runLoginAuthPassword1307SelfTests() -> bool
{
    bool ok = true;

    ok = expect(loginHelpers::isBcryptHash(makeBcrypt('a')), "version a accepted") && ok;
    ok = expect(loginHelpers::isBcryptHash(makeBcrypt('b')), "version b accepted") && ok;
    ok = expect(loginHelpers::isBcryptHash(makeBcrypt('y')), "version y accepted") && ok;
    ok = expect(loginHelpers::isBcryptHash(makeBcrypt('x')), "version x accepted") && ok;

    ok = expect(!loginHelpers::isBcryptHash(""), "empty rejected") && ok;
    ok = expect(!loginHelpers::isBcryptHash(makeBcrypt('a').substr(0, 59)), "length 59 rejected") && ok;
    ok = expect(!loginHelpers::isBcryptHash(makeBcrypt('a') + "Z"), "length 61 rejected") && ok;
    ok = expect(!loginHelpers::isBcryptHash(makeBcrypt('c')), "version c rejected") && ok;
    ok = expect(!loginHelpers::isBcryptHash("*A41B000000000000000000000000000000000000000000000000000000"), "Maria PASSWORD style rejected") && ok;

    {
        std::string almost = makeBcrypt('a');
        almost[0]          = '#';
        ok = expect(!loginHelpers::isBcryptHash(almost), "bad first dollar rejected") && ok;
    }
    {
        std::string almost = makeBcrypt('a');
        almost[1]          = '3';
        ok = expect(!loginHelpers::isBcryptHash(almost), "bad second char rejected") && ok;
    }
    {
        std::string almost = makeBcrypt('a');
        almost[3]          = '#';
        ok = expect(!loginHelpers::isBcryptHash(almost), "bad cost separator rejected") && ok;
    }

    using gate = loginHelpers::login_attempt_account_gate;
    ok = expect(loginHelpers::classifyLoginAttemptAccountStatus(ACCOUNT_STATUS_CODE::NORMAL) == gate::ALLOW,
                "NORMAL allows") &&
         ok;
    ok = expect(loginHelpers::classifyLoginAttemptAccountStatus(ACCOUNT_STATUS_CODE::BANNED) == gate::REJECT_BANNED,
                "BANNED rejects with purge") &&
         ok;
    ok = expect(loginHelpers::classifyLoginAttemptAccountStatus(0) == gate::REJECT, "zero status rejects") && ok;
    ok = expect(loginHelpers::classifyLoginAttemptAccountStatus(0x04) == gate::REJECT, "unknown flag rejects") && ok;
    ok = expect(loginHelpers::classifyLoginAttemptAccountStatus(
                    static_cast<uint32>(ACCOUNT_STATUS_CODE::NORMAL) | static_cast<uint32>(ACCOUNT_STATUS_CODE::BANNED)) ==
                    gate::ALLOW,
                "NORMAL wins over BANNED") &&
         ok;

    return ok;
}
