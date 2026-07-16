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

#include <array>
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

// Keep every source table entry pinned. The grade API intentionally accepts
// only defined row and column indices; grades.cpp directly indexes its arrays.
auto testAllTableLookups() -> bool
{
    constexpr std::array<std::array<uint8, 9>, 23> expectedJobGrades = { {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 2, 0, 1, 3, 4, 3, 6, 6, 5 }, { 1, 0, 3, 2, 1, 6, 7, 4, 5 },
        { 5, 3, 4, 6, 4, 5, 5, 1, 3 }, { 6, 2, 6, 3, 6, 3, 1, 5, 4 }, { 4, 4, 4, 4, 5, 5, 3, 3, 4 },
        { 4, 0, 4, 1, 4, 2, 3, 7, 7 }, { 3, 6, 2, 5, 1, 7, 7, 3, 3 }, { 3, 6, 1, 3, 3, 4, 3, 7, 7 },
        { 3, 0, 4, 3, 4, 6, 5, 5, 1 }, { 4, 0, 4, 4, 4, 6, 4, 4, 2 }, { 5, 0, 5, 4, 4, 1, 5, 4, 5 },
        { 2, 0, 3, 3, 3, 4, 5, 5, 4 }, { 4, 0, 3, 2, 3, 2, 4, 7, 6 }, { 3, 0, 2, 4, 3, 4, 6, 5, 3 },
        { 7, 1, 6, 5, 6, 4, 2, 2, 2 }, { 4, 4, 5, 5, 5, 5, 5, 5, 5 }, { 4, 0, 5, 3, 5, 2, 3, 5, 5 },
        { 4, 0, 5, 2, 4, 3, 5, 6, 3 }, { 4, 0, 4, 3, 5, 2, 6, 6, 2 }, { 5, 4, 6, 4, 5, 4, 3, 4, 3 },
        { 3, 2, 6, 4, 5, 4, 3, 3, 4 }, { 3, 6, 3, 4, 5, 2, 4, 4, 6 },
    } };
    constexpr std::array<std::array<uint8, 9>, 5> expectedRaceGrades = { {
        { 4, 4, 4, 4, 4, 4, 4, 4, 4 }, { 3, 5, 2, 5, 3, 6, 6, 2, 4 }, { 7, 1, 6, 4, 5, 3, 1, 5, 4 },
        { 4, 4, 5, 1, 5, 2, 4, 5, 6 }, { 1, 7, 3, 4, 1, 5, 5, 4, 6 },
    } };
    constexpr std::array<std::array<float, 5>, 8> expectedHPScale = { {
        { 0, 0, 0, 0, 0 }, { 19, 9, 1, 3, 3 }, { 17, 8, 1, 3, 3 }, { 16, 7, 1, 3, 3 },
        { 14, 6, 0, 3, 3 }, { 13, 5, 0, 2, 2 }, { 11, 4, 0, 2, 2 }, { 10, 3, 0, 2, 2 },
    } };
    constexpr std::array<std::array<float, 4>, 8> expectedMPScale = { {
        { 0, 0, 0, 0 }, { 16, 6, 4, 0 }, { 14, 5, 4, 0 }, { 12, 4, 4, 0 },
        { 10, 3, 4, 0 }, { 8, 2, 3, 0 }, { 6, 1, 2, 0 }, { 4, 0.5F, 1, 0 },
    } };
    constexpr std::array<std::array<float, 4>, 8> expectedStatScale = { {
        { 0, 0, 0, 0 }, { 5, 0.5F, 0.1F, 0.35F }, { 4, 0.45F, 0.2F, 0.35F }, { 4, 0.4F, 0.25F, 0.35F },
        { 3, 0.35F, 0.35F, 0.35F }, { 3, 0.3F, 0.35F, 0.35F }, { 2, 0.25F, 0.4F, 0.35F }, { 2, 0.2F, 0.4F, 0.35F },
    } };
    constexpr std::array<std::array<uint8, 3>, 8> expectedMobHPScale = { {
        { 0, 0, 0 }, { 36, 9, 1 }, { 33, 8, 1 }, { 32, 7, 1 }, { 29, 6, 0 }, { 27, 5, 0 }, { 24, 4, 0 }, { 22, 3, 0 },
    } };

    for (uint8 row = 0; row < expectedJobGrades.size(); ++row)
        for (uint8 column = 0; column < expectedJobGrades[row].size(); ++column)
            if (grade::GetJobGrade(static_cast<JOBTYPE>(row), column) != expectedJobGrades[row][column]) return false;
    for (uint8 row = 0; row < expectedRaceGrades.size(); ++row)
        for (uint8 column = 0; column < expectedRaceGrades[row].size(); ++column)
            if (grade::GetRaceGrades(row, column) != expectedRaceGrades[row][column]) return false;
    for (uint8 row = 0; row < expectedHPScale.size(); ++row)
        for (uint8 column = 0; column < expectedHPScale[row].size(); ++column)
            if (!expectFloat(grade::GetHPScale(row, column), expectedHPScale[row][column], "HP scale table")) return false;
    for (uint8 row = 0; row < expectedMPScale.size(); ++row)
        for (uint8 column = 0; column < expectedMPScale[row].size(); ++column)
            if (!expectFloat(grade::GetMPScale(row, column), expectedMPScale[row][column], "MP scale table")) return false;
    for (uint8 row = 0; row < expectedStatScale.size(); ++row)
        for (uint8 column = 0; column < expectedStatScale[row].size(); ++column)
            if (!expectFloat(grade::GetStatScale(row, column), expectedStatScale[row][column], "stat scale table")) return false;
    for (uint8 row = 0; row < expectedMobHPScale.size(); ++row)
        for (uint8 column = 0; column < expectedMobHPScale[row].size(); ++column)
            if (grade::GetMobHPScale(row, column) != expectedMobHPScale[row][column]) return false;
    return true;
}

} // namespace

auto runGradesSelfTests() -> bool
{
    bool ok = true;
    ok      = testJobTypeValues() && ok;
    ok      = testGradeLookups() && ok;
    ok      = testScaleLookups() && ok;
    ok      = testAllTableLookups() && ok;
    return ok;
}
