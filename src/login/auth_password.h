#pragma once

#include "auth_session.h"

#include <cstddef>
#include <string>

// Pure password-scheme and LOGIN_ATTEMPT account-status helpers for AUTH.
// Extracted so native tests can pin the exact predicates without SSL/DB hosts.

namespace loginHelpers
{

// isBcryptHash mirrors the anonymous-namespace helper formerly local to
// auth_session::validatePassword. A 60-byte Modular Crypt Format bcrypt string
// with version a/b/y/x is treated as bcrypt; everything else uses MariaDB
// PASSWORD() with optional migration.
inline auto isBcryptHash(const std::string& passHash) -> bool
{
    return std::size(passHash) == 60 &&
           passHash[0] == '$' &&
           passHash[1] == '2' &&
           (passHash[2] == 'a' || passHash[2] == 'b' || passHash[2] == 'y' || passHash[2] == 'x') &&
           passHash[3] == '$';
}

// login_attempt_account_gate is the pure outcome of the LOGIN_ATTEMPT status
// check after password validation succeeds.
enum class login_attempt_account_gate : uint8_t
{
    ALLOW,         // status has NORMAL — continue OTP/session work
    REJECT,        // non-NORMAL and not BANNED — LOGIN_FAIL only
    REJECT_BANNED, // non-NORMAL and BANNED — purge trust tokens, then LOGIN_FAIL
};

// classifyLoginAttemptAccountStatus mirrors the status gate in
// auth_session::read_func LOGIN_ATTEMPT. NORMAL is checked first; when NORMAL
// is set the account is allowed even if BANNED is also set.
inline auto classifyLoginAttemptAccountStatus(const uint32 status) -> login_attempt_account_gate
{
    if (status & ACCOUNT_STATUS_CODE::NORMAL)
    {
        return login_attempt_account_gate::ALLOW;
    }
    if (status & ACCOUNT_STATUS_CODE::BANNED)
    {
        return login_attempt_account_gate::REJECT_BANNED;
    }
    return login_attempt_account_gate::REJECT;
}

} // namespace loginHelpers
