/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_grades.h"

#include "map/grades.h"

#include <cmath>
#include <cstdint>
#include <iostream>

namespace
{

auto expectUInt(std::uint32_t actual, std::uint32_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "grades self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectFloat(float actual, float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001F)
    {
        std::cerr << "grades self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testJobTypeValues() -> bool
{
    bool ok = true;
    ok      = expectUInt(static_cast<std::uint32_t>(JOB_NON), 0, "JOB_NON") && ok;
    ok      = expectUInt(static_cast<std::uint32_t>(JOB_WAR), 1, "JOB_WAR") && ok;
    ok      = expectUInt(static_cast<std::uint32_t>(JOB_SMN), 15, "JOB_SMN") && ok;
    ok      = expectUInt(static_cast<std::uint32_t>(JOB_RUN), 22, "JOB_RUN") && ok;
    return ok;
}

auto testGradeLookups() -> bool
{
    bool ok = true;
    ok      = expectUInt(grade::GetJobGrade(JOB_NON, 0), 0, "none HP grade") && ok;
    ok      = expectUInt(grade::GetJobGrade(JOB_WAR, 2), 1, "war STR grade") && ok;
    ok      = expectUInt(grade::GetJobGrade(JOB_WHM, 7), 1, "whm MND grade") && ok;
    ok      = expectUInt(grade::GetJobGrade(JOB_SMN, 0), 7, "smn HP grade") && ok;
    ok      = expectUInt(grade::GetJobGrade(JOB_RUN, 8), 6, "run CHR grade") && ok;

    ok = expectUInt(grade::GetRaceGrades(0, 0), 4, "hume HP grade") && ok;
    ok = expectUInt(grade::GetRaceGrades(1, 3), 5, "elvaan DEX grade") && ok;
    ok = expectUInt(grade::GetRaceGrades(2, 1), 1, "tarutaru MP grade") && ok;
    ok = expectUInt(grade::GetRaceGrades(3, 8), 6, "mithra CHR grade") && ok;
    ok = expectUInt(grade::GetRaceGrades(4, 4), 1, "galka VIT grade") && ok;
    return ok;
}

auto testScaleLookups() -> bool
{
    bool ok = true;
    ok      = expectFloat(grade::GetHPScale(1, 0), 19.0F, "HPScale rank A base") && ok;
    ok      = expectFloat(grade::GetHPScale(7, 4), 2.0F, "HPScale rank G >75") && ok;
    ok      = expectFloat(grade::GetMPScale(7, 1), 0.5F, "MPScale rank G <60") && ok;
    ok      = expectFloat(grade::GetMPScale(1, 3), 0.0F, "MPScale rank A zero-filled column") && ok;
    ok      = expectFloat(grade::GetStatScale(1, 2), 0.10F, "StatScale rank A <75") && ok;
    ok      = expectFloat(grade::GetStatScale(7, 3), 0.35F, "StatScale rank G >75") && ok;
    ok      = expectUInt(grade::GetMobHPScale(1, 0), 36, "MobHPScale rank A base") && ok;
    ok      = expectUInt(grade::GetMobHPScale(7, 2), 0, "MobHPScale rank G scaleX") && ok;
    return ok;
}

} // namespace

auto runGradesSelfTests() -> bool
{
    bool ok = true;
    ok      = testJobTypeValues() && ok;
    ok      = testGradeLookups() && ok;
    ok      = testScaleLookups() && ok;
    return ok;
}
