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

#include "test_tracy_hex_helpers.h"

#include "common/tracy.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace
{

struct HexCase
{
    std::string label;
    std::string actual;
    std::string expected;
};

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Tracy hex helper self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testHex8ToString() -> bool
{
    const std::vector<HexCase> tests{
        { "hex8 zero", hex8ToString(static_cast<std::uint8_t>(0x00)), "0x00" },
        { "hex8 one", hex8ToString(static_cast<std::uint8_t>(0x01)), "0x01" },
        { "hex8 uppercase", hex8ToString(static_cast<std::uint8_t>(0x0A)), "0x0A" },
        { "hex8 high bit", hex8ToString(static_cast<std::uint8_t>(0x80)), "0x80" },
        { "hex8 max", hex8ToString(static_cast<std::uint8_t>(0xFF)), "0xFF" },
    };

    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualString(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto testHex16ToString() -> bool
{
    const std::vector<HexCase> tests{
        { "hex16 zero", hex16ToString(static_cast<std::uint16_t>(0x0000)), "0x0000" },
        { "hex16 one", hex16ToString(static_cast<std::uint16_t>(0x0001)), "0x0001" },
        { "hex16 uppercase", hex16ToString(static_cast<std::uint16_t>(0x00AB)), "0x00AB" },
        { "hex16 normal", hex16ToString(static_cast<std::uint16_t>(0x1234)), "0x1234" },
        { "hex16 max", hex16ToString(static_cast<std::uint16_t>(0xFFFF)), "0xFFFF" },
    };

    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualString(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto testHex32ToString() -> bool
{
    const std::vector<HexCase> tests{
        { "hex32 zero", hex32ToString(static_cast<std::uint32_t>(0x00000000)), "0x00000000" },
        { "hex32 one", hex32ToString(static_cast<std::uint32_t>(0x00000001)), "0x00000001" },
        { "hex32 uppercase", hex32ToString(static_cast<std::uint32_t>(0x0000ABCD)), "0x0000ABCD" },
        { "hex32 normal", hex32ToString(static_cast<std::uint32_t>(0x12345678)), "0x12345678" },
        { "hex32 max", hex32ToString(static_cast<std::uint32_t>(0xFFFFFFFF)), "0xFFFFFFFF" },
    };

    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualString(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

} // namespace

auto runTracyHexHelperSelfTests() -> bool
{
    return testHex8ToString() && testHex16ToString() && testHex32ToString();
}
