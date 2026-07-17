#pragma once

#include "auth_password.h"

// Pure AUTH LOGIN_ATTEMPT success control-flow after OTP verification:
// satchel sync gate + success JSON field plan + trust-token issuance gate.
// MD5 session hash, token generation, SQL, and JSON send remain host-owned.

namespace loginHelpers
{

// ShouldSyncSatchelOnOTP mirrors the otpVerified gate before
// UPDATE char_storage SET satchel = inventory for account chars.
inline auto ShouldSyncSatchelOnOTP(const bool otpVerified) -> bool
{
    return otpVerified;
}

// login_success_reply_plan describes which LOGIN_ATTEMPT success JSON fields
// the host must populate. Hash bytes and trust-token string remain host-owned.
struct login_success_reply_plan
{
    login_result result{};              // always LoginAttemptSuccessResult
    bool         includeAccountId{};    // always true
    bool         includeSessionHash{};  // always true
    bool         includeTrustToken{};   // iff issueTrustToken
};

// PlanLoginSuccessReply builds the success JSON field plan after OTP path
// success. issueTrustToken is typically ShouldIssueTrustToken(...).
inline auto PlanLoginSuccessReply(const bool issueTrustToken) -> login_success_reply_plan
{
    return {
        .result             = LoginAttemptSuccessResult,
        .includeAccountId   = true,
        .includeSessionHash = true,
        .includeTrustToken  = issueTrustToken,
    };
}

// login_success_plan is the full AUTH success control-flow plan after OTP
// verification: satchel sync + reply fields composed with ShouldIssueTrustToken.
struct login_success_plan
{
    bool                     syncSatchel{};
    login_success_reply_plan reply{};
};

// PlanLoginSuccess composes satchel sync and reply planning with
// ShouldIssueTrustToken for the post-OTP success path.
inline auto PlanLoginSuccess(const bool otpVerified, const bool trustThisComputer) -> login_success_plan
{
    return {
        .syncSatchel = ShouldSyncSatchelOnOTP(otpVerified),
        .reply       = PlanLoginSuccessReply(ShouldIssueTrustToken(trustThisComputer, otpVerified)),
    };
}

} // namespace loginHelpers
