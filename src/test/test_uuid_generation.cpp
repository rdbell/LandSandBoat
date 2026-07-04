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

#include "test_uuid_generation.h"

#include "common/uuid.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "uuid generation self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectTrue(const bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "uuid generation self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto isDashPosition(const std::size_t index) -> bool
{
    constexpr std::array<std::size_t, 4> positions{ 8, 13, 18, 23 };
    for (const auto position : positions)
    {
        if (index == position)
        {
            return true;
        }
    }

    return false;
}

auto isLowerHex(const char c) -> bool
{
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f');
}

auto testUUIDShape(const std::string& value, const std::string& label) -> bool
{
    bool ok = true;

    ok = expectEqual(value.size(), std::size_t{ 36 }, label + " length") && ok;
    if (!ok)
    {
        return false;
    }

    for (std::size_t i = 0; i < value.size(); ++i)
    {
        if (isDashPosition(i))
        {
            ok = expectEqual(value[i], '-', label + " dash " + std::to_string(i)) && ok;
        }
        else
        {
            ok = expectTrue(isLowerHex(value[i]), label + " hex " + std::to_string(i)) && ok;
        }
    }

    return ok;
}

} // namespace

auto runUUIDGenerationSelfTests() -> bool
{
    bool ok = true;

    ok = testUUIDShape(uuid::GenerateUUID(), "single") && ok;

    for (int i = 0; i < 8; ++i)
    {
        ok = testUUIDShape(uuid::GenerateUUID(), "generated " + std::to_string(i)) && ok;
    }

    return ok;
}
