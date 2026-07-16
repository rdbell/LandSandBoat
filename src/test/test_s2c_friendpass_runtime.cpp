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

#include "test_s2c_friendpass_runtime.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string_view>
#include <utility>

#include "map/packets/s2c/0x059_friendpass.h"

namespace
{

auto expect(bool condition, std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "s2c FRIENDPASS runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto testZeroPassPlan() -> bool
{
    const auto plan = friendpasshelpers::PlanFor(0);
    return expect(plan.leftNum == 0 && plan.leftDays == 0 && plan.passPop == 10000 && plan.Type == 0x03 && plan.unknown21 == 0x01 && plan.padding00 == 0, "zero-pass defaults") &&
           expect(std::memcmp(plan.String, std::array<char, 16>{}.data(), sizeof(plan.String)) == 0, "zero-pass string");
}

auto testSuppliedPassPlans() -> bool
{
    bool ok = true;
    for (const auto& [worldPass, expected] : std::array{
             std::pair<uint32, std::string_view>{ 1, "0000000001" },
             std::pair<uint32, std::string_view>{ 12345, "0000012345" },
             std::pair<uint32, std::string_view>{ UINT32_MAX, "4294967295" },
         })
    {
        const auto plan = friendpasshelpers::PlanFor(worldPass);
        ok              = expect(plan.leftNum == 1 && plan.leftDays == 167 && plan.passPop == 10000 && plan.Type == 0x06 && plan.unknown21 == 0x01 && plan.padding00 == 0, "supplied-pass field overrides") && ok;
        ok              = expect(std::string_view(plan.String, expected.size()) == expected, "supplied-pass ten-digit formatting") && ok;
        ok              = expect(std::memcmp(plan.String + expected.size(), std::array<char, 16 - 10>{}.data(), 16 - expected.size()) == 0, "supplied-pass string padding") && ok;
    }
    return ok;
}

} // namespace

auto runS2CFriendPassRuntimeSelfTests() -> bool
{
    return testZeroPassPlan() && testSuppliedPassPlans();
}
