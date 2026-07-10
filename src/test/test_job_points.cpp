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

#include "test_job_points.h"

#include "map/job_points.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectU16(std::uint16_t actual, std::uint16_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "job points self-test failed: " << label << " got " << actual
                  << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectU8(std::uint8_t actual, std::uint8_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "job points self-test failed: " << label << " got "
                  << static_cast<unsigned>(actual) << " expected "
                  << static_cast<unsigned>(expected) << '\n';
        return false;
    }
    return true;
}

auto testJobPointCost() -> bool
{
    bool ok = true;
    ok      = expectU8(JobPointCost(0), 1, "cost(0)") && ok;
    ok      = expectU8(JobPointCost(1), 2, "cost(1)") && ok;
    ok      = expectU8(JobPointCost(19), 20, "cost(19)") && ok;
    ok      = expectU8(JobPointCost(20), 0, "cost(20) blocks raise") && ok;
    ok      = expectU8(JobPointCost(21), 1, "cost(21) wraps") && ok;
    return ok;
}

auto testCategoryAndTypeIndexMacros() -> bool
{
    bool ok = true;

    ok = expectU16(JobPointsCategoryByJobId(1), JPCATEGORY_WAR, "category WAR") && ok;
    ok = expectU16(JobPointsCategoryByJobId(2), JPCATEGORY_MNK, "category MNK") && ok;
    ok = expectU16(JobPointsCategoryByJobId(22), JPCATEGORY_RUN, "category RUN") && ok;

    ok = expectU16(JobPointsCategoryIndexByJpType(JP_MIGHTY_STRIKES_EFFECT), 1, "index WAR") && ok;
    ok = expectU16(JobPointsCategoryIndexByJpType(JP_ELEMENTAL_SFORZO_EFFECT), 22, "index RUN") && ok;

    ok = expectU8(JobPointTypeIndex(JP_MIGHTY_STRIKES_EFFECT), 0, "type index 0") && ok;
    ok = expectU8(JobPointTypeIndex(JP_BERSERK_EFFECT), 1, "type index 1") && ok;
    ok = expectU8(JobPointTypeIndex(JP_BRAZEN_RUSH_EFFECT), 2, "type index 2") && ok;
    ok = expectU8(JobPointTypeIndex(JP_DOUBLE_ATTACK_EFFECT), 9, "type index 9") && ok;

    return ok;
}

auto testCategoryConstants() -> bool
{
    bool ok = true;
    ok      = expectU16(JOBPOINTS_CATEGORY_COUNT, 22, "category count") && ok;
    ok      = expectU16(JOBPOINTS_CATEGORY_START, 0x020, "category start") && ok;
    ok      = expectU16(JOBPOINTS_JPTYPE_COUNT, 220, "type count") && ok;
    ok      = expectU16(JOBPOINTS_JPTYPE_PER_CATEGORY, 10, "types per category") && ok;
    ok      = expectU16(JOBPOINTS_MAX, 500, "max job points") && ok;
    ok      = expectU16(JOBPOINTS_CAPACITY_MAX, 30000, "capacity max") && ok;
    return ok;
}

} // namespace

auto runJobPointsSelfTests() -> bool
{
    bool ok = true;
    ok      = testJobPointCost() && ok;
    ok      = testCategoryAndTypeIndexMacros() && ok;
    ok      = testCategoryConstants() && ok;
    return ok;
}
