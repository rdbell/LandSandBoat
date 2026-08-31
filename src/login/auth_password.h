#pragma once

#include "auth_session.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure password-scheme, LOGIN_ATTEMPT account-status, AUTH diagnostic, and
// LOGIN_CREATE account-id / pre-insert gate helpers. Extracted so native tests
// can pin the exact predicates without SSL/DB hosts.

namespace loginHelpers
{

// MinAccountID is the floor applied after max_id + 1 in LOGIN_CREATE.
// Retail-era account IDs start at 1000.
constexpr uint32 MinAccountID = 1000;

// NextAccountID mirrors LOGIN_CREATE: candidate = maxExistingID + 1 (uint32
// wrap), then floor at MinAccountID when candidate is still below the floor.
inline auto NextAccountID(const uint32 maxExistingID) -> uint32
{
    const uint32 candidate = maxExistingID + 1;
    return candidate < MinAccountID ? MinAccountID : candidate;
}

// account_create_settings_gate is the pure outcome of the
// login.ACCOUNT_CREATION settings check at the top of LOGIN_CREATE.
enum class account_create_settings_gate : uint8_t
{
    ALLOW,    // creation enabled — continue username lookup
    DISABLED, // creation disabled — LOGIN_ERROR_CREATE_DISABLED
};

// ClassifyAccountCreationSettings mirrors the ACCOUNT_CREATION settings gate.
inline auto ClassifyAccountCreationSettings(const bool creationEnabled) -> account_create_settings_gate
{
    return creationEnabled ? account_create_settings_gate::ALLOW : account_create_settings_gate::DISABLED;
}

// LoginResultForAccountCreateSettings maps the settings gate to a reply code.
// Returns nullopt when creation may proceed.
inline auto LoginResultForAccountCreateSettings(const account_create_settings_gate gate) -> Maybe<login_result>
{
    if (gate == account_create_settings_gate::DISABLED)
    {
        return login_result::LOGIN_ERROR_CREATE_DISABLED;
    }
    return std::nullopt;
}

// FormatAccountCreateDisabledWarning preserves the LOGIN_CREATE diagnostic
// emitted when account creation is disabled by settings.
inline auto FormatAccountCreateDisabledWarning(const std::string& username) -> std::string
{
    return fmt::format("login_parse: New account attempt <{}> but is disabled in settings.", username);
}

// account_create_lookup_gate is the pure outcome of the username existence
// query used by LOGIN_CREATE before allocating an ID.
enum class account_create_lookup_gate : uint8_t
{
    PROCEED,      // query ok and no matching login — create
    TAKEN,        // query ok and login already exists
    QUERY_FAILED, // prepared statement failed
};

// ClassifyAccountCreateLookup mirrors the SELECT accounts.id by login gate.
// rowsCount is only meaningful when queryOk is true.
inline auto ClassifyAccountCreateLookup(const bool queryOk, const std::size_t rowsCount) -> account_create_lookup_gate
{
    if (!queryOk)
    {
        return account_create_lookup_gate::QUERY_FAILED;
    }
    if (rowsCount == 0)
    {
        return account_create_lookup_gate::PROCEED;
    }
    return account_create_lookup_gate::TAKEN;
}

// LoginResultForAccountCreateLookup maps the lookup gate to a reply code.
// Returns nullopt when creation may proceed to max-id / insert.
inline auto LoginResultForAccountCreateLookup(const account_create_lookup_gate gate) -> Maybe<login_result>
{
    switch (gate)
    {
        case account_create_lookup_gate::TAKEN:
            return login_result::LOGIN_ERROR_CREATE_TAKEN;
        case account_create_lookup_gate::QUERY_FAILED:
            return login_result::LOGIN_ERROR_CREATE;
        case account_create_lookup_gate::PROCEED:
            return std::nullopt;
    }
    return login_result::LOGIN_ERROR_CREATE;
}

// FormatUnsupportedXiloaderVersionError mirrors the JSON error_message body
// sent when validateAuthInput reports VERSION_UNSUPPORTED. Patch is rendered
// as the literal 'x' for the supported version and as the reported client
// patch for the third reported component.
inline auto FormatUnsupportedXiloaderVersionError(const std::array<uint8_t, 3>& clientVersion) -> std::string
{
    return fmt::format(
        "Your xiloader is too old.\nPlease update to version '{}.{}.x'.\nYour client reported '{}.{}.{}'.",
        SupportedXiloaderVersion[0],
        SupportedXiloaderVersion[1],
        clientVersion[0],
        clientVersion[1],
        clientVersion[2]);
}

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

// login_attempt_otp_outcome is the pure LOGIN_ATTEMPT OTP/trust-token plan
// after the account-status gate allows login.
enum class login_attempt_otp_outcome : uint8_t
{
    SKIP,                 // account does not need OTP — continue, otpVerified=false
    VERIFIED,             // OTP path succeeded — continue, otpVerified=true
    REJECT_TRUST_TOKEN,   // invalid/missing trust with no OTP fallback — TRUST_TOKEN_INVALID
    REJECT_OTP,           // TOTP validation failed — LOGIN_ERROR
};

// login_attempt_otp_plan pairs the outcome with the otpVerified flag used for
// satchel upgrade and optional trust-token issuance.
struct login_attempt_otp_plan
{
    login_attempt_otp_outcome outcome{};
    bool                      otpVerified{};
};

// PlanLoginAttemptOTP mirrors the doesAccountNeedOTP / trust_token / otp block
// in LOGIN_ATTEMPT. Host injects trustTokenValid (only meaningful when
// trustToken is non-empty) and totpValid (only consulted when not trusted).
inline auto PlanLoginAttemptOTP(
    const bool        needsOTP,
    const std::string& trustToken,
    const std::string& otp,
    const bool        trustTokenValid,
    const bool        totpValid) -> login_attempt_otp_plan
{
    if (!needsOTP)
    {
        return login_attempt_otp_plan{ .outcome = login_attempt_otp_outcome::SKIP, .otpVerified = false };
    }

    const bool trustedByToken = !trustToken.empty() && trustTokenValid;
    if (!trustedByToken)
    {
        if (otp.empty() && !trustToken.empty())
        {
            return login_attempt_otp_plan{ .outcome = login_attempt_otp_outcome::REJECT_TRUST_TOKEN, .otpVerified = false };
        }
        if (!totpValid)
        {
            return login_attempt_otp_plan{ .outcome = login_attempt_otp_outcome::REJECT_OTP, .otpVerified = false };
        }
    }

    return login_attempt_otp_plan{ .outcome = login_attempt_otp_outcome::VERIFIED, .otpVerified = true };
}

// LoginResultForLoginAttemptOTP maps reject outcomes to reply codes. Returns
// nullopt when login may continue (SKIP or VERIFIED).
inline auto LoginResultForLoginAttemptOTP(const login_attempt_otp_outcome outcome) -> Maybe<login_result>
{
    switch (outcome)
    {
        case login_attempt_otp_outcome::REJECT_TRUST_TOKEN:
            return login_result::LOGIN_ERROR_TRUST_TOKEN_INVALID;
        case login_attempt_otp_outcome::REJECT_OTP:
            return login_result::LOGIN_ERROR;
        case login_attempt_otp_outcome::SKIP:
        case login_attempt_otp_outcome::VERIFIED:
            return std::nullopt;
    }
    return login_result::LOGIN_ERROR;
}

// ShouldIssueTrustToken mirrors trust_this_computer && otpVerified on success.
inline auto ShouldIssueTrustToken(const bool trustThisComputer, const bool otpVerified) -> bool
{
    return trustThisComputer && otpVerified;
}

// change_password_status_plan is the pure dual-flag outcome of the
// LOGIN_CHANGE_PASSWORD status checks after password/OTP validation.
// LSB checks BANNED and NORMAL independently: a banned emit does not return,
// so NORMAL accounts that are also BANNED still enter the update path after
// an early LOGIN_ERROR_CHANGE_PASSWORD reply.
struct change_password_status_plan
{
    bool emitBannedError{}; // status & BANNED — send LOGIN_ERROR_CHANGE_PASSWORD (no return)
    bool attemptUpdate{};   // status & NORMAL — enter empty-password / UPDATE path
};

// PlanChangePasswordStatus mirrors the two independent bit tests in
// LOGIN_CHANGE_PASSWORD.
inline auto PlanChangePasswordStatus(const uint32 status) -> change_password_status_plan
{
    return change_password_status_plan{
        .emitBannedError = (status & ACCOUNT_STATUS_CODE::BANNED) != 0,
        .attemptUpdate   = (status & ACCOUNT_STATUS_CODE::NORMAL) != 0,
    };
}

// IsEmptyUpdatedPassword mirrors the updated_password == "" check before
// hashing and writing a new password on LOGIN_CHANGE_PASSWORD.
inline auto IsEmptyUpdatedPassword(const std::string& updatedPassword) -> bool
{
    return updatedPassword == "";
}

// FormatEmptyPasswordWarning preserves the LOGIN_CHANGE_PASSWORD diagnostic
// emitted when the new password is empty.
inline auto FormatEmptyPasswordWarning(const std::string& username) -> std::string
{
    return fmt::format("login_parse: Empty password: Could not update password for user <{}>.", username);
}

// FormatPasswordUpdateDatabaseError preserves the diagnostic emitted when the
// LOGIN_CHANGE_PASSWORD UPDATE statement fails.
inline auto FormatPasswordUpdateDatabaseError(const std::string& username) -> std::string
{
    return fmt::format("login_parse: Error trying to update password in database for user <{}>.", username);
}

// TOTP URI fixed parameters embedded by LOGIN_CREATE_TOTP (not settings-driven).
inline constexpr const char* TOTPURIAlgorithm = "SHA1";
inline constexpr int         TOTPURIDigits    = 6;
inline constexpr int         TOTPURIPeriod    = 30;

// FormatTOTPURI mirrors the otpauth URI built by LOGIN_CREATE_TOTP after a
// non-empty secret is created. Values are interpolated without URL encoding,
// matching LSB fmt::format usage. Issuer is always the server name.
inline auto FormatTOTPURI(const std::string& serverName, const std::string& username, const std::string& secret) -> std::string
{
    return fmt::format("otpauth://totp/{}:{}?secret={}&issuer={}&algorithm={}&digits={}&period={}",
                       serverName,
                       username,
                       secret,
                       serverName,
                       TOTPURIAlgorithm,
                       TOTPURIDigits,
                       TOTPURIPeriod);
}

// IsCreateTOTPSecretValid reports whether createAccountSecret produced a usable
// secret. Empty secrets take the failure JSON path in LOGIN_CREATE_TOTP.
inline auto IsCreateTOTPSecretValid(const std::string& secret) -> bool
{
    return !secret.empty();
}

// FailedCredentialValidationMessage is the error_message body used by several
// AUTH TOTP/credential failure paths in auth_session.
inline constexpr const char* FailedCredentialValidationMessage = "Failed to validate credentials";

// MatchesRecoveryCode mirrors strcmpi(otp.c_str(), recoveryCode.c_str()) == 0
// used by LOGIN_REMOVE_TOTP and LOGIN_REGENERATE_RECOVERY. Comparison is
// case-insensitive and length-sensitive (null-terminated C-string rules via
// strcmpi / strcasecmp).
inline auto MatchesRecoveryCode(const std::string& otp, const std::string& recoveryCode) -> bool
{
    return strcmpi(otp.c_str(), recoveryCode.c_str()) == 0;
}

// AcceptsOTPOrRecovery mirrors the dual validation gate:
//   validateTOTP(otp, secret) || MatchesRecoveryCode(otp, recoveryCode)
// totpValid is injected so the pure half does not depend on wall-clock TOTP.
inline auto AcceptsOTPOrRecovery(const bool totpValid, const std::string& otp, const std::string& recoveryCode) -> bool
{
    return totpValid || MatchesRecoveryCode(otp, recoveryCode);
}

// SessionHashSeed mirrors the LOGIN_ATTEMPT success hash input:
//   earth_time::timestamp() ^ getpid()
// before MD5 over the native 4-byte representation of the seed.
inline auto SessionHashSeed(const uint32 timestamp, const uint32 pid) -> uint32
{
    return timestamp ^ pid;
}

// SessionHashLength is the MD5 digest size stored as session_hash (JSON array).
constexpr std::size_t SessionHashLength = 16;

// Change-password success JSON fixed fields (LOGIN_CHANGE_PASSWORD).
constexpr login_result ChangePasswordSuccessResult    = login_result::LOGIN_SUCCESS_CHANGE_PASSWORD;
constexpr uint32       ChangePasswordSuccessAccountID = 0;
// Empty string session_hash (not a 16-byte array) on change-password success.
inline constexpr const char* ChangePasswordSuccessSessionHash = "";

// Login-attempt success JSON result field.
constexpr login_result LoginAttemptSuccessResult = login_result::LOGIN_SUCCESS;

// FormatUnhandledAuthCode mirrors the default login_cmd switch diagnostic.
// code is the raw JSON "command" value (int8 in production).
inline auto FormatUnhandledAuthCode(const int8 code, const std::string& ipAddress) -> std::string
{
    return fmt::format("Unhandled auth code: {} from {}", code, ipAddress);
}

} // namespace loginHelpers
