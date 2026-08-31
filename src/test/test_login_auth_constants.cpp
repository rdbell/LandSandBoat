/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_login_auth_constants.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "login/auth_session.h"
#include "omega_self_test_registry.h"

namespace
{

struct ConstantCase
{
    std::uint64_t actual;
    std::uint64_t expected;
    std::string   label;
};

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login auth constant self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectAll(const std::vector<ConstantCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto testLoginCommands() -> bool
{
    return expectAll({
        { static_cast<std::uint64_t>(login_cmd::LOGIN_NOOP), 0x00, "login_cmd::LOGIN_NOOP" },
        { static_cast<std::uint64_t>(login_cmd::LOGIN_ATTEMPT), 0x10, "login_cmd::LOGIN_ATTEMPT" },
        { static_cast<std::uint64_t>(login_cmd::LOGIN_CREATE), 0x20, "login_cmd::LOGIN_CREATE" },
        { static_cast<std::uint64_t>(login_cmd::LOGIN_CHANGE_PASSWORD), 0x30, "login_cmd::LOGIN_CHANGE_PASSWORD" },
        { static_cast<std::uint64_t>(login_cmd::LOGIN_CREATE_TOTP), 0x31, "login_cmd::LOGIN_CREATE_TOTP" },
        { static_cast<std::uint64_t>(login_cmd::LOGIN_REMOVE_TOTP), 0x32, "login_cmd::LOGIN_REMOVE_TOTP" },
        { static_cast<std::uint64_t>(login_cmd::LOGIN_REGENERATE_RECOVERY), 0x33, "login_cmd::LOGIN_REGENERATE_RECOVERY" },
        { static_cast<std::uint64_t>(login_cmd::LOGIN_VERIFY_TOTP), 0x34, "login_cmd::LOGIN_VERIFY_TOTP" },
    });
}

auto testLoginResults() -> bool
{
    return expectAll({
        { static_cast<std::uint64_t>(login_result::LOGIN_FAIL), 0x00, "login_result::LOGIN_FAIL" },
        { static_cast<std::uint64_t>(login_result::LOGIN_SUCCESS), 0x01, "login_result::LOGIN_SUCCESS" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR), 0x02, "login_result::LOGIN_ERROR" },
        { static_cast<std::uint64_t>(login_result::LOGIN_SUCCESS_CREATE), 0x03, "login_result::LOGIN_SUCCESS_CREATE" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR_CREATE_TAKEN), 0x04, "login_result::LOGIN_ERROR_CREATE_TAKEN" },
        { static_cast<std::uint64_t>(login_result::LOGIN_REQUEST_NEW_PASSWORD), 0x05, "login_result::LOGIN_REQUEST_NEW_PASSWORD" },
        { static_cast<std::uint64_t>(login_result::LOGIN_SUCCESS_CHANGE_PASSWORD), 0x06, "login_result::LOGIN_SUCCESS_CHANGE_PASSWORD" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR_CHANGE_PASSWORD), 0x07, "login_result::LOGIN_ERROR_CHANGE_PASSWORD" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR_CREATE_DISABLED), 0x08, "login_result::LOGIN_ERROR_CREATE_DISABLED" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR_CREATE), 0x09, "login_result::LOGIN_ERROR_CREATE" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR_ALREADY_LOGGED_IN), 0x0A, "login_result::LOGIN_ERROR_ALREADY_LOGGED_IN" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR_VERSION_UNSUPPORTED), 0x0B, "login_result::LOGIN_ERROR_VERSION_UNSUPPORTED" },
        { static_cast<std::uint64_t>(login_result::LOGIN_SUCCESS_CREATE_TOTP), 0x10, "login_result::LOGIN_SUCCESS_CREATE_TOTP" },
        { static_cast<std::uint64_t>(login_result::LOGIN_SUCCESS_VERIFY_TOTP), 0x11, "login_result::LOGIN_SUCCESS_VERIFY_TOTP" },
        { static_cast<std::uint64_t>(login_result::LOGIN_SUCCESS_REMOVE_TOTP), 0x12, "login_result::LOGIN_SUCCESS_REMOVE_TOTP" },
        { static_cast<std::uint64_t>(login_result::LOGIN_ERROR_TRUST_TOKEN_INVALID), 0x13, "login_result::LOGIN_ERROR_TRUST_TOKEN_INVALID" },
    });
}

auto testSupportedXiloaderVersion() -> bool
{
    bool ok = true;
    ok = expectEqualInt(SupportedXiloaderVersion[0], 2, "SupportedXiloaderVersion major") && ok;
    ok = expectEqualInt(SupportedXiloaderVersion[1], 1, "SupportedXiloaderVersion minor") && ok;
    ok = expectEqualInt(SupportedXiloaderVersion[2], 0, "SupportedXiloaderVersion patch") && ok;
    return ok;
}

auto testAuthInputValidation() -> bool
{
    return expectAll({
        { static_cast<std::uint64_t>(malformedAuthPacketResult(0x00)), static_cast<std::uint64_t>(login_result::LOGIN_ERROR), "ancient malformed packet" },
        { static_cast<std::uint64_t>(malformedAuthPacketResult(0xFF)), static_cast<std::uint64_t>(login_result::LOGIN_ERROR_VERSION_UNSUPPORTED), "old malformed packet" },
        { static_cast<std::uint64_t>(isSupportedXiloaderVersion({ 2, 1, 0 })), 1, "supported exact version" },
        { static_cast<std::uint64_t>(isSupportedXiloaderVersion({ 2, 1, 255 })), 1, "patch version ignored" },
        { static_cast<std::uint64_t>(isSupportedXiloaderVersion({ 2, 0, 255 })), 0, "minor version rejected" },
        { static_cast<std::uint64_t>(validateAuthInput({ 1, 0, 0 }, "", "")), static_cast<std::uint64_t>(auth_input_validation::VERSION_UNSUPPORTED), "version validation precedes credentials" },
        { static_cast<std::uint64_t>(validateAuthInput({ 2, 1, 0 }, "", "")), static_cast<std::uint64_t>(auth_input_validation::MALFORMED_USERNAME), "username validation precedes password" },
        { static_cast<std::uint64_t>(validateAuthInput({ 2, 1, 7 }, "username", "")), static_cast<std::uint64_t>(auth_input_validation::MALFORMED_PASSWORD), "malformed password" },
        { static_cast<std::uint64_t>(validateAuthInput({ 2, 1, 7 }, "username", "password")), static_cast<std::uint64_t>(auth_input_validation::READY), "ready input" },
    });
}

auto testAuthComponents() -> bool
{
    const auto combined = SEND_EMAIL | ENABLE_ACCOUNT_CREATE | ENABLE_PASSWORD_RESET;
    return expectAll({
        { static_cast<std::uint64_t>(SEND_EMAIL), 1U << 0, "SEND_EMAIL" },
        { static_cast<std::uint64_t>(SEND_HOSTNAME), 1U << 1, "SEND_HOSTNAME" },
        { static_cast<std::uint64_t>(SEND_MAC_ADDRESS), 1U << 2, "SEND_MAC_ADDRESS" },
        { static_cast<std::uint64_t>(ENABLE_ACCOUNT_CREATE), 1U << 3, "ENABLE_ACCOUNT_CREATE" },
        { static_cast<std::uint64_t>(ENABLE_ACCOUNT_DELETE), 1U << 4, "ENABLE_ACCOUNT_DELETE" },
        { static_cast<std::uint64_t>(ENABLE_PASSWORD_CHANGE), 1U << 5, "ENABLE_PASSWORD_CHANGE" },
        { static_cast<std::uint64_t>(ENABLE_PASSWORD_RESET), 1U << 6, "ENABLE_PASSWORD_RESET" },
        { static_cast<std::uint64_t>(combined), (1U << 0) | (1U << 3) | (1U << 6), "AUTH_COMPONENTS combined" },
    });
}

auto testAccountCodes() -> bool
{
    const auto status = ACCOUNT_STATUS_CODE::NORMAL | ACCOUNT_STATUS_CODE::BANNED;
    return expectAll({
        { static_cast<std::uint64_t>(ACCOUNT_STATUS_CODE::NORMAL), 0x01, "ACCOUNT_STATUS_CODE::NORMAL" },
        { static_cast<std::uint64_t>(ACCOUNT_STATUS_CODE::BANNED), 0x02, "ACCOUNT_STATUS_CODE::BANNED" },
        { static_cast<std::uint64_t>(status), 0x03, "ACCOUNT_STATUS_CODE combined" },
        { static_cast<std::uint64_t>(ACCOUNT_PRIVILEGE_CODE::USER), 0x01, "ACCOUNT_PRIVILEGE_CODE::USER" },
        { static_cast<std::uint64_t>(ACCOUNT_PRIVILEGE_CODE::ADMIN), 0x02, "ACCOUNT_PRIVILEGE_CODE::ADMIN" },
        { static_cast<std::uint64_t>(ACCOUNT_PRIVILEGE_CODE::ROOT), 0x04, "ACCOUNT_PRIVILEGE_CODE::ROOT" },
    });
}

} // namespace

auto runLoginAuthConstantSelfTests() -> bool
{
    bool ok = testLoginCommands() &&
              testLoginResults() &&
              testSupportedXiloaderVersion() &&
              testAuthInputValidation() &&
              testAuthComponents() &&
              testAccountCodes();
    ok = loginHelpers::FormatMalformedUsernameWarning("127.0.0.1") == "login_parse: malformed username from 127.0.0.1" && ok;
    ok = loginHelpers::FormatMalformedPasswordWarning("127.0.0.1") == "login_parse: malformed password from 127.0.0.1" && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("login-auth-diagnostics-8221", runLoginAuthConstantSelfTests);
