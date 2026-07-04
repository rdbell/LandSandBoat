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

#include "test_login_malformed_string.h"

#include "login/login_helpers.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

struct Case
{
    std::string input;
    std::size_t maxLength;
    bool        expected;
    std::string label;
};

auto expectEqual(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login malformed string self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runLoginMalformedStringSelfTests() -> bool
{
    bool ok = true;

    const std::vector<Case> cases{
        { "Omega", 5, false, "valid exact max" },
        { "Omega", 6, false, "valid under max" },
        { "", 5, true, "empty" },
        { "Omega", 4, true, "too long" },
        { "Omega XI", 8, false, "space accepted" },
        { std::string{ 'O', 'm', '\x1f' }, 3, true, "unit separator rejected" },
        { std::string{ 'O', 'm', '\n' }, 3, true, "newline rejected" },
        { std::string{ 'O', 'm', '\0' }, 3, true, "nul rejected" },
        { std::string{ 'O', 'm', '\x7f' }, 3, false, "delete accepted" },
        { std::string{ 'O', static_cast<char>(0xff) }, 2, static_cast<char>(0xff) < 0x20, "high byte follows char signedness" },
    };

    for (const auto& test : cases)
    {
        ok = expectEqual(loginHelpers::isStringMalformed(test.input, test.maxLength), test.expected, test.label) && ok;
    }

    return ok;
}
