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

#include "test_c2s_packet_id_enum.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include "map/enums/packet_c2s.h"

namespace
{

struct EnumCase
{
    PacketC2S     actual;
    std::uint16_t expected;
    std::string   label;
};

auto enumValue(PacketC2S value) -> std::uint16_t
{
    return static_cast<std::uint16_t>(value);
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s packet id enum self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s packet id enum self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s packet id enum self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectAll(const std::vector<EnumCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(enumValue(test.actual), test.expected, test.label) && ok;
    }
    return ok;
}

auto testRepresentativePacketIDValues() -> bool
{
    return expectAll({
        { PacketC2S::GP_CLI_COMMAND_LOGIN, 0x00A, "GP_CLI_COMMAND_LOGIN" },
        { PacketC2S::GP_CLI_COMMAND_GAMEOK, 0x00C, "GP_CLI_COMMAND_GAMEOK" },
        { PacketC2S::GP_CLI_COMMAND_ZONE_TRANSITION, 0x011, "GP_CLI_COMMAND_ZONE_TRANSITION" },
        { PacketC2S::GP_CLI_COMMAND_ITEMSEARCH, 0x02C, "GP_CLI_COMMAND_ITEMSEARCH" },
        { PacketC2S::GP_CLI_COMMAND_EVENTENDXZY, 0x05C, "GP_CLI_COMMAND_EVENTENDXZY" },
        { PacketC2S::GP_CLI_COMMAND_PASSWARDS, 0x060, "GP_CLI_COMMAND_PASSWARDS" },
        { PacketC2S::GP_CLI_COMMAND_GROUP_CHECKID, 0x078, "GP_CLI_COMMAND_GROUP_CHECKID" },
        { PacketC2S::GP_CLI_COMMAND_SHOP_SELL_SET, 0x085, "GP_CLI_COMMAND_SHOP_SELL_SET" },
        { PacketC2S::GP_CLI_COMMAND_JOB_POINTS_REQ, 0x0C0, "GP_CLI_COMMAND_JOB_POINTS_REQ" },
        { PacketC2S::GP_CLI_COMMAND_GET_LSPRIV, 0x0E4, "GP_CLI_COMMAND_GET_LSPRIV" },
        { PacketC2S::GP_CLI_COMMAND_MYROOM_JOB, 0x100, "GP_CLI_COMMAND_MYROOM_JOB" },
        { PacketC2S::GP_CLI_COMMAND_CURRENCIES_2, 0x115, "GP_CLI_COMMAND_CURRENCIES_2" },
        { PacketC2S::GP_CLI_COMMAND_JUMP, 0x11D, "GP_CLI_COMMAND_JUMP" },
    });
}

auto testMagicEnumRangeAndContains() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint64_t>(magic_enum::customize::enum_range<PacketC2S>::min), 0, "PacketC2S magic_enum min") && ok;
    ok = expectEqualInt(static_cast<std::uint64_t>(magic_enum::customize::enum_range<PacketC2S>::max), 300, "PacketC2S magic_enum max") && ok;
    ok = expectEqualInt(static_cast<std::uint64_t>(magic_enum::enum_count<PacketC2S>()), 130, "PacketC2S magic_enum count") && ok;

    ok = expectTrue(magic_enum::enum_contains(PacketC2S::GP_CLI_COMMAND_LOGIN), "magic_enum contains LOGIN") && ok;
    ok = expectTrue(magic_enum::enum_contains(static_cast<PacketC2S>(0x085)), "magic_enum contains SHOP_SELL_SET by value") && ok;
    ok = expectTrue(magic_enum::enum_contains(PacketC2S::GP_CLI_COMMAND_JUMP), "magic_enum contains JUMP") && ok;

    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketC2S>(0x000)), "magic_enum excludes range-min hole") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketC2S>(0x00B)), "magic_enum excludes sparse hole 0x00B") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketC2S>(0x11A)), "magic_enum excludes sparse hole 0x11A") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketC2S>(300)), "magic_enum excludes range-max hole") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketC2S>(301)), "magic_enum excludes value beyond customized range") && ok;

    return ok;
}

auto testCompletePacketIDSet() -> bool
{
    constexpr std::array<std::uint16_t, 130> expected = {
        0x00A, 0x00C, 0x00D, 0x00F, 0x011, 0x015, 0x016, 0x017, 0x01A, 0x01B,
        0x01C, 0x01E, 0x01F, 0x028, 0x029, 0x02B, 0x02C, 0x032, 0x033, 0x034,
        0x036, 0x037, 0x03A, 0x03B, 0x03C, 0x03D, 0x041, 0x042, 0x04B, 0x04D,
        0x04E, 0x050, 0x051, 0x052, 0x053, 0x058, 0x059, 0x05A, 0x05B, 0x05C,
        0x05D, 0x05E, 0x060, 0x061, 0x063, 0x064, 0x066, 0x06E, 0x06F, 0x070,
        0x071, 0x074, 0x076, 0x077, 0x078, 0x083, 0x084, 0x085, 0x096, 0x09B,
        0x0A0, 0x0A1, 0x0A2, 0x0AA, 0x0AB, 0x0AC, 0x0AD, 0x0B5, 0x0B6, 0x0B7,
        0x0BE, 0x0BF, 0x0C0, 0x0C1, 0x0C3, 0x0C4, 0x0CB, 0x0D2, 0x0D3, 0x0D4,
        0x0D5, 0x0D8, 0x0DB, 0x0DC, 0x0DD, 0x0DE, 0x0E0, 0x0E1, 0x0E2, 0x0E4,
        0x0E7, 0x0E8, 0x0EA, 0x0EB, 0x0F0, 0x0F1, 0x0F2, 0x0F4, 0x0F5, 0x0F6,
        0x0FA, 0x0FB, 0x0FC, 0x0FD, 0x0FE, 0x0FF, 0x100, 0x102, 0x104, 0x105,
        0x106, 0x109, 0x10A, 0x10B, 0x10C, 0x10D, 0x10E, 0x10F, 0x110, 0x112,
        0x113, 0x114, 0x115, 0x116, 0x117, 0x118, 0x119, 0x11B, 0x11C, 0x11D,
    };
    constexpr auto actual = magic_enum::enum_values<PacketC2S>();

    bool ok = expectEqualInt(actual.size(), expected.size(), "complete PacketC2S set count");
    for (std::size_t index = 0; index < expected.size() && index < actual.size(); ++index)
    {
        ok = expectEqualInt(enumValue(actual[index]), expected[index], "complete PacketC2S set index " + std::to_string(index)) && ok;
    }
    return ok;
}

} // namespace

auto runC2SPacketIDEnumSelfTests() -> bool
{
    bool ok = true;

    ok = testRepresentativePacketIDValues() && ok;
    ok = testMagicEnumRangeAndContains() && ok;
    ok = testCompletePacketIDSet() && ok;

    return ok;
}
