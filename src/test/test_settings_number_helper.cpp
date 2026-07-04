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

#include "test_settings_number_helper.h"

#include <iostream>
#include <string>
#include <vector>

namespace settings
{
bool isNumber(const std::string& stringValue);
} // namespace settings

namespace
{

struct Case
{
    std::string input;
    bool        expected;
};

auto expectEqual(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "settings number helper self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runSettingsNumberHelperSelfTests() -> bool
{
    bool ok = true;

    const std::vector<Case> cases{
        { "", true },
        { "0", true },
        { "1234567890", true },
        { "00123", true },
        { "-1", false },
        { "12.3", false },
        { "12 3", false },
        { "abc", false },
    };

    for (const auto& test : cases)
    {
        ok = expectEqual(settings::isNumber(test.input), test.expected, test.input) && ok;
    }

    return ok;
}
