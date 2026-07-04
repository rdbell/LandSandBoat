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

#include "test_zlib_compressed_size.h"

#include "common/zlib.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

namespace
{

struct Case
{
    std::size_t size;
    std::size_t expected;
};

auto expectEqual(const std::size_t actual, const std::size_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "zlib compressed size self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runZlibCompressedSizeSelfTests() -> bool
{
    bool ok = true;

    constexpr std::array<Case, 11> cases{ {
        { 0, 0 },
        { 1, 1 },
        { 7, 1 },
        { 8, 1 },
        { 9, 2 },
        { 15, 2 },
        { 16, 2 },
        { 17, 3 },
        { 63, 8 },
        { 64, 8 },
        { 65, 9 },
    } };

    for (const auto& test : cases)
    {
        ok = expectEqual(zlib_compressed_size(test.size), test.expected, "size " + std::to_string(test.size)) && ok;
    }

    return ok;
}
