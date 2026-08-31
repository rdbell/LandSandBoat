/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_login_trust_token_8320.h"

#include "login/otp_helpers.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login trust token 8320 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginTrustToken8320SelfTests() -> bool
{
    bool ok = true;
    const std::string canonical = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

    const unsigned char bytes[] = { 0x00, 0x01, 0xAB, 0xFF };
    ok = expect(otpHelpers::toHexString(bytes, sizeof(bytes)) == "0001abff", "lowercase hexadecimal encoding") && ok;

    ok = expect(!otpHelpers::isValidTrustTokenFormat(""), "empty token rejected") && ok;
    ok = expect(!otpHelpers::isValidTrustTokenFormat(canonical.substr(0, 63)), "short token rejected") && ok;
    ok = expect(otpHelpers::isValidTrustTokenFormat(canonical), "lowercase hexadecimal token accepted") && ok;
    ok = expect(otpHelpers::isValidTrustTokenFormat("ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789"),
                "uppercase hexadecimal token accepted") &&
         ok;
    ok = expect(!otpHelpers::isValidTrustTokenFormat(canonical.substr(0, 63) + "g"), "non hexadecimal token rejected") && ok;

    ok = expect(otpHelpers::hashTrustToken("") ==
                    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
                "empty token SHA-256") &&
         ok;
    ok = expect(otpHelpers::hashTrustToken(canonical) ==
                    "a8ae6e6ee929abea3afcfc5258c8ccd6f85273e0d4626d26c7279f3250f77c8e",
                "canonical token SHA-256") &&
         ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("login-trust-token-8320", runLoginTrustToken8320SelfTests);
