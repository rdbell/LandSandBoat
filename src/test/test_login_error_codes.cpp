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

#include "test_login_error_codes.h"

#include "login/login_errors.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace
{

struct ErrorCodeCase
{
    loginErrors::errorCode code;
    std::uint16_t          expected;
    std::string            label;
};

auto expectEqualInt(std::uint16_t actual, std::uint16_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login error code self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testLoginErrorCodes() -> bool
{
    const std::vector<ErrorCodeCase> tests{
        { loginErrors::UNABLE_TO_CONNECT_TO_WORLD_SERVER, 305, "UNABLE_TO_CONNECT_TO_WORLD_SERVER" },
        { loginErrors::CHARACTER_NAME_UNAVAILABLE, 313, "CHARACTER_NAME_UNAVAILABLE" },
        { loginErrors::CHARACTER_ALREADY_LOGGED_IN, 201, "CHARACTER_ALREADY_LOGGED_IN" },
        { loginErrors::WORLD_IS_FULL, 208, "WORLD_IS_FULL" },
        { loginErrors::FAILED_TO_REGISTER_WITH_THE_NAME_SERVER, 314, "FAILED_TO_REGISTER_WITH_THE_NAME_SERVER" },
        { loginErrors::CHARACTERS_PARAMETERS_ARE_INCORRECT, 321, "CHARACTERS_PARAMETERS_ARE_INCORRECT" },
        { loginErrors::GAMES_DATA_HAS_BEEN_UPDATED, 331, "GAMES_DATA_HAS_BEEN_UPDATED" },
        { loginErrors::COULD_NOT_CONNECT_TO_LOBBY_SERVER, 332, "COULD_NOT_CONNECT_TO_LOBBY_SERVER" },
    };

    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(static_cast<std::uint16_t>(test.code), test.expected, test.label) && ok;
    }
    return ok;
}

} // namespace

auto runLoginErrorCodeSelfTests() -> bool
{
    return testLoginErrorCodes();
}
