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

#include "test_login_otp_helpers.h"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace otpHelpers
{
std::vector<uint8_t> base32Decode(const std::string& base32);
std::string          generateTOTP(const std::string& base32Secret, uint64_t epochSeconds, int digits = 6, int period = 30);
} // namespace otpHelpers

namespace
{

auto expectEqual(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login OTP helpers self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectBytes(const std::vector<uint8_t>& actual, const std::string& expected, const std::string& label) -> bool
{
    return expectEqual(std::string(actual.begin(), actual.end()), expected, label);
}

auto testBase32Decode() -> bool
{
    bool ok = true;

    ok = expectBytes(otpHelpers::base32Decode(""), "", "empty decode") && ok;
    ok = expectBytes(otpHelpers::base32Decode("JBSWY3DPEB3W64TMMQ======"), "Hello world", "hello world") && ok;
    ok = expectBytes(otpHelpers::base32Decode("jbsw y3dp-eb3w_64tmmq"), "Hello world", "lowercase separators") && ok;
    ok = expectBytes(otpHelpers::base32Decode("MY"), "f", "partial byte discarded") && ok;

    bool threw = false;
    try
    {
        (void)otpHelpers::base32Decode("JBSW!");
    }
    catch (const std::runtime_error&)
    {
        threw = true;
    }
    ok = expectEqual(threw ? "throw" : "no throw", "throw", "invalid character") && ok;

    return ok;
}

auto testGenerateTOTP() -> bool
{
    bool ok = true;

    const auto secret = std::string("GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ");

    ok = expectEqual(otpHelpers::generateTOTP(secret, 59), "287082", "default totp") && ok;
    ok = expectEqual(otpHelpers::generateTOTP(secret, 59, 8, 30), "00287082", "eight digit width") && ok;
    ok = expectEqual(otpHelpers::generateTOTP(secret, 59, 10, 30), "000028708", "large width truncation") && ok;
    ok = expectEqual(otpHelpers::generateTOTP(secret, 59, 12, 30), "000000287", "larger width truncation") && ok;
    ok = expectEqual(otpHelpers::generateTOTP(secret, 1111111109), "081804", "later vector") && ok;
    ok = expectEqual(otpHelpers::generateTOTP(secret, 59, 6, 60), "755224", "custom period") && ok;

    bool threw = false;
    try
    {
        (void)otpHelpers::generateTOTP("BAD!", 59);
    }
    catch (const std::runtime_error&)
    {
        threw = true;
    }
    ok = expectEqual(threw ? "throw" : "no throw", "throw", "invalid totp secret") && ok;

    return ok;
}

} // namespace

auto runLoginOTPHelpersSelfTests() -> bool
{
    bool ok = true;

    ok = testBase32Decode() && ok;
    ok = testGenerateTOTP() && ok;

    return ok;
}
