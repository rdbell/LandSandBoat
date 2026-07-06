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

#include "test_mmo_stats_skills.h"

#include "common/mmo.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo stats/skills self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo stats/skills self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << " expected";
        for (const auto value : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

template <typename T>
auto bytesOf(const T& value) -> std::array<std::uint8_t, sizeof(T)>
{
    std::array<std::uint8_t, sizeof(T)> bytes{};
    std::memcpy(bytes.data(), &value, bytes.size());
    return bytes;
}

template <typename Struct, typename Field>
auto fieldOffset(const Struct& value, const Field& field) -> std::uint64_t
{
    return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&field) - reinterpret_cast<std::uintptr_t>(&value));
}

auto testStatsLayoutAndBytes() -> bool
{
    bool ok = true;

    stats_t stats{};
    ok = expectEqualUInt(sizeof(stats_t), 14, "stats_t sizeof") && ok;
    ok = expectEqualUInt(fieldOffset(stats, stats.STR), 0, "stats_t STR offset") && ok;
    ok = expectEqualUInt(fieldOffset(stats, stats.DEX), 2, "stats_t DEX offset") && ok;
    ok = expectEqualUInt(fieldOffset(stats, stats.VIT), 4, "stats_t VIT offset") && ok;
    ok = expectEqualUInt(fieldOffset(stats, stats.AGI), 6, "stats_t AGI offset") && ok;
    ok = expectEqualUInt(fieldOffset(stats, stats.INT), 8, "stats_t INT offset") && ok;
    ok = expectEqualUInt(fieldOffset(stats, stats.MND), 10, "stats_t MND offset") && ok;
    ok = expectEqualUInt(fieldOffset(stats, stats.CHR), 12, "stats_t CHR offset") && ok;

    stats.STR = 0x1001;
    stats.DEX = 0x1002;
    stats.VIT = 0x1003;
    stats.AGI = 0x1004;
    stats.INT = 0x1005;
    stats.MND = 0x1006;
    stats.CHR = 0x1007;
    ok        = expectBytes(bytesOf(stats),
                            std::array<std::uint8_t, 14>{
                                0x01, 0x10,
                                0x02, 0x10,
                                0x03, 0x10,
                                0x04, 0x10,
                                0x05, 0x10,
                                0x06, 0x10,
                                0x07, 0x10,
                            },
                            "stats_t golden bytes") &&
         ok;

    return ok;
}

auto testSkillsLayout() -> bool
{
    bool ok = true;

    skills_t skills{};
    ok = expectEqualUInt(sizeof(skills_t), 192, "skills_t sizeof") && ok;
    ok = expectEqualUInt(sizeof(skills.skill) / sizeof(skills.skill[0]), 64, "skills_t skill count") && ok;
    ok = expectEqualUInt(sizeof(skills.rank) / sizeof(skills.rank[0]), 64, "skills_t rank count") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.skill), 0, "skills_t skill offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.rank), 128, "skills_t rank offset") && ok;

    ok = expectEqualUInt(fieldOffset(skills, skills.unknown1), 0, "skills_t unknown1 offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.h2h), 2, "skills_t h2h offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.staff), 24, "skills_t staff offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.reserved1), 26, "skills_t reserved1 offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.automaton_melee), 44, "skills_t automaton_melee offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.healing), 66, "skills_t healing offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.enhancing), 68, "skills_t enhancing offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.blue), 86, "skills_t blue offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.geomancy), 88, "skills_t geomancy offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.handbell), 90, "skills_t handbell offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.reserved2), 92, "skills_t reserved2 offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.fishing), 96, "skills_t fishing offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.riding), 116, "skills_t riding offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.reserved3), 118, "skills_t reserved3 offset") && ok;
    ok = expectEqualUInt(fieldOffset(skills, skills.unknown2), 126, "skills_t unknown2 offset") && ok;

    return ok;
}

auto testSkillsUnionAliasesAndBytes() -> bool
{
    bool ok = true;

    skills_t skills{};
    skills.h2h              = 0x1111;
    skills.dagger           = 0x2222;
    skills.reserved1[8]     = 0x3333;
    skills.automaton_magic  = 0x4444;
    skills.healing          = 0x5555;
    skills.blue             = 0x6666;
    skills.geomancy         = 0x7777;
    skills.handbell         = 0x8888;
    skills.reserved2[1]     = 0x9999;
    skills.fishing          = 0xAAAA;
    skills.riding           = 0xBBBB;
    skills.reserved3[3]     = 0xCCCC;
    skills.unknown2         = 0xDDDD;
    skills.rank[48]         = 0x12;
    skills.rank[63]         = 0x34;

    ok = expectEqualUInt(skills.skill[1], 0x1111, "h2h aliases skill[1]") && ok;
    ok = expectEqualUInt(skills.skill[2], 0x2222, "dagger aliases skill[2]") && ok;
    ok = expectEqualUInt(skills.skill[21], 0x3333, "reserved1 aliases skill[21]") && ok;
    ok = expectEqualUInt(skills.skill[24], 0x4444, "automaton_magic aliases skill[24]") && ok;
    ok = expectEqualUInt(skills.skill[33], 0x5555, "healing aliases skill[33]") && ok;
    ok = expectEqualUInt(skills.skill[43], 0x6666, "blue aliases skill[43]") && ok;
    ok = expectEqualUInt(skills.skill[44], 0x7777, "geomancy aliases skill[44]") && ok;
    ok = expectEqualUInt(skills.skill[45], 0x8888, "handbell aliases skill[45]") && ok;
    ok = expectEqualUInt(skills.skill[47], 0x9999, "reserved2 aliases skill[47]") && ok;
    ok = expectEqualUInt(skills.skill[48], 0xAAAA, "fishing aliases skill[48]") && ok;
    ok = expectEqualUInt(skills.skill[58], 0xBBBB, "riding aliases skill[58]") && ok;
    ok = expectEqualUInt(skills.skill[62], 0xCCCC, "reserved3 aliases skill[62]") && ok;
    ok = expectEqualUInt(skills.skill[63], 0xDDDD, "unknown2 aliases skill[63]") && ok;

    skills.skill[34] = 0x1357;
    ok               = expectEqualUInt(skills.enhancing, 0x1357, "skill[34] aliases enhancing") && ok;

    auto bytes = bytesOf(skills);
    ok         = expectEqualUInt(bytes[2], 0x11, "skills_t h2h low byte") && ok;
    ok         = expectEqualUInt(bytes[3], 0x11, "skills_t h2h high byte") && ok;
    ok         = expectEqualUInt(bytes[68], 0x57, "skills_t enhancing low byte") && ok;
    ok         = expectEqualUInt(bytes[69], 0x13, "skills_t enhancing high byte") && ok;
    ok         = expectEqualUInt(bytes[96], 0xAA, "skills_t fishing low byte") && ok;
    ok         = expectEqualUInt(bytes[97], 0xAA, "skills_t fishing high byte") && ok;
    ok         = expectEqualUInt(bytes[128 + 48], 0x12, "skills_t rank[48] byte") && ok;
    ok         = expectEqualUInt(bytes[128 + 63], 0x34, "skills_t rank[63] byte") && ok;

    return ok;
}

} // namespace

auto runMMOStatsSkillsSelfTests() -> bool
{
    return testStatsLayoutAndBytes() &&
           testSkillsLayout() &&
           testSkillsUnionAliasesAndBytes();
}
