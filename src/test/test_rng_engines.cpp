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

#include "test_rng_engines.h"

#include "common/rng/null.h"
#include "common/rng/squirrel5.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "rng engines self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto testSquirrel5Noise() -> bool
{
    bool ok = true;

    ok = expectEqual(Squirrel5::noise(0, 0x12345678U), 3905667014U, "Squirrel5::noise position 0") && ok;
    ok = expectEqual(Squirrel5::noise(1, 0x12345678U), 2198521095U, "Squirrel5::noise position 1") && ok;
    ok = expectEqual(Squirrel5::noise(2, 0x12345678U), 257106568U, "Squirrel5::noise position 2") && ok;
    ok = expectEqual(Squirrel5::noise(12345, 0x12345678U), 4197225990U, "Squirrel5::noise position 12345") && ok;
    ok = expectEqual(Squirrel5::noise(-1, 0x12345678U), 3730284402U, "Squirrel5::noise position -1") && ok;

    return ok;
}

auto testSquirrel5Sequence() -> bool
{
    bool ok = true;

    ok = expectEqual(Squirrel5::min(), 0U, "Squirrel5 min") && ok;
    ok = expectEqual(Squirrel5::max(), std::numeric_limits<uint32_t>::max(), "Squirrel5 max") && ok;

    {
        Squirrel5 engine(0);
        constexpr std::array<uint32_t, 6> expected = {
            377036288U,
            3365260061U,
            3009420505U,
            2388214638U,
            3194367137U,
            2470340015U,
        };

        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            ok = expectEqual(engine(), expected[i], "Squirrel5 seed zero sequence") && ok;
        }
    }

    {
        Squirrel5 engine(0xDEADBEEFU);
        constexpr std::array<uint32_t, 6> expected = {
            506427052U,
            4151581038U,
            4250317417U,
            1656452875U,
            1094824577U,
            677078434U,
        };

        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            ok = expectEqual(engine(), expected[i], "Squirrel5 seeded sequence") && ok;
        }

        engine.seed(0xDEADBEEFU);
        constexpr std::array<uint32_t, 3> resetExpected = {
            506427052U,
            4151581038U,
            4250317417U,
        };

        for (std::size_t i = 0; i < resetExpected.size(); ++i)
        {
            ok = expectEqual(engine(), resetExpected[i], "Squirrel5 reset sequence") && ok;
        }
    }

    return ok;
}

auto testNullRandomEngine() -> bool
{
    bool ok = true;
    NullRandomEngine engine;

    ok = expectEqual(NullRandomEngine::min(), 0ULL, "NullRandomEngine min") && ok;
    ok = expectEqual(NullRandomEngine::max(), std::numeric_limits<uint64_t>::max(), "NullRandomEngine max") && ok;
    ok = expectEqual(engine(), 9223372036854775807ULL, "NullRandomEngine first value") && ok;

    engine.seed(12345);
    ok = expectEqual(engine(), 9223372036854775807ULL, "NullRandomEngine after seed") && ok;

    return ok;
}

} // namespace

auto runRngEnginesSelfTests() -> bool
{
    bool ok = true;

    ok = testSquirrel5Noise() && ok;
    ok = testSquirrel5Sequence() && ok;
    ok = testNullRandomEngine() && ok;

    return ok;
}
