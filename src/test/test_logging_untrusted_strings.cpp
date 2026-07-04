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

#include "test_logging_untrusted_strings.h"

#include "common/logging.h"

#include <array>
#include <iostream>
#include <string>

namespace
{

auto expectEqual(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "logging untrusted strings self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testNullPointers() -> bool
{
    const char* nullChar = nullptr;
    return expectEqual(asStringFromUntrustedSource(nullChar), "", "null default") &&
           expectEqual(asStringFromUntrustedSource(nullChar, 4), "", "null max");
}

auto testEmbeddedNullsAndCaps() -> bool
{
    bool ok = true;

    const char embeddedNull[] = { 'O', 'm', '\0', 'e', 'g', 'a' };
    ok = expectEqual(asStringFromUntrustedSource(embeddedNull), "Om", "embedded null default") && ok;
    ok = expectEqual(asStringFromUntrustedSource(embeddedNull, sizeof(embeddedNull)), "Om", "embedded null max") && ok;

    const char unterminated[] = { 'O', 'm', 'e', 'g', 'a' };
    ok = expectEqual(asStringFromUntrustedSource(unterminated, sizeof(unterminated)), "Omega", "unterminated bounded") && ok;
    ok = expectEqual(asStringFromUntrustedSource(unterminated, 2), "Om", "caller cap") && ok;
    ok = expectEqual(asStringFromUntrustedSource(unterminated, 0), "", "zero cap") && ok;

    return ok;
}

auto testDefaultMaxLength() -> bool
{
    std::array<char, 1030> data{};
    data.fill('x');

    bool ok = true;
    ok = expectEqual(asStringFromUntrustedSource(data.data()), std::string(1024, 'x'), "default max length") && ok;
    ok = expectEqual(asStringFromUntrustedSource(data.data(), 1028), std::string(1028, 'x'), "caller max beyond default") && ok;

    return ok;
}

auto testNonCharBuffers() -> bool
{
    bool ok = true;

    const unsigned char highBytes[] = { 0xFF, 'A', '\0' };
    auto                expected   = std::string{};
    expected.push_back(static_cast<char>(0xFF));
    expected.push_back('A');
    ok = expectEqual(asStringFromUntrustedSource(highBytes), expected, "unsigned char buffer") && ok;

    return ok;
}

} // namespace

auto runLoggingUntrustedStringsSelfTests() -> bool
{
    bool ok = true;

    ok = testNullPointers() && ok;
    ok = testEmbeddedNullsAndCaps() && ok;
    ok = testDefaultMaxLength() && ok;
    ok = testNonCharBuffers() && ok;

    return ok;
}
