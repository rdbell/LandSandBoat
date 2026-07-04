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

#include "test_s2c_packet_id_enum.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include "map/enums/packet_s2c.h"

namespace
{

struct EnumCase
{
    PacketS2C     actual;
    std::uint16_t expected;
    std::string   label;
};

auto enumValue(PacketS2C value) -> std::uint16_t
{
    return static_cast<std::uint16_t>(value);
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c packet id enum self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "s2c packet id enum self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "s2c packet id enum self-test failed: " << label << '\n';
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
        { PacketS2C::GP_SERV_COMMAND_PACKETCONTROL, 0x005, "GP_SERV_COMMAND_PACKETCONTROL" },
        { PacketS2C::GP_SERV_COMMAND_ENTERZONE, 0x008, "GP_SERV_COMMAND_ENTERZONE" },
        { PacketS2C::GP_SERV_COMMAND_CHAR_NPC, 0x00E, "GP_SERV_COMMAND_CHAR_NPC" },
        { PacketS2C::GP_SERV_COMMAND_ITEM_TRADE_MYLIST, 0x025, "GP_SERV_COMMAND_ITEM_TRADE_MYLIST" },
        { PacketS2C::GP_SERV_COMMAND_BATTLE_MESSAGE2, 0x02D, "GP_SERV_COMMAND_BATTLE_MESSAGE2" },
        { PacketS2C::GP_SERV_COMMAND_SCHEDULOR, 0x038, "GP_SERV_COMMAND_SCHEDULOR" },
        { PacketS2C::GP_SERV_COMMAND_EVENTUCOFF, 0x052, "GP_SERV_COMMAND_EVENTUCOFF" },
        { PacketS2C::GP_SERV_COMMAND_CLISTATUS2, 0x062, "GP_SERV_COMMAND_CLISTATUS2" },
        { PacketS2C::GP_SERV_COMMAND_UNKNOWN_074, 0x074, "GP_SERV_COMMAND_UNKNOWN_074" },
        { PacketS2C::GP_SERV_COMMAND_GUILD_SELLLIST, 0x085, "GP_SERV_COMMAND_GUILD_SELLLIST" },
        { PacketS2C::GP_SERV_PACKET_ALTER_EGO_POINTS, 0x08E, "GP_SERV_PACKET_ALTER_EGO_POINTS" },
        { PacketS2C::GP_SERV_COMMAND_MAGIC_DATA, 0x0AA, "GP_SERV_COMMAND_MAGIC_DATA" },
        { PacketS2C::GP_SERV_COMMAND_GMSCITEM, 0x0B7, "GP_SERV_COMMAND_GMSCITEM" },
        { PacketS2C::GP_SERV_COMMAND_GROUP_CHECKID, 0x0E1, "GP_SERV_COMMAND_GROUP_CHECKID" },
        { PacketS2C::GP_SERV_COMMAND_MYROOM_OPERATION, 0x0FA, "GP_SERV_COMMAND_MYROOM_OPERATION" },
        { PacketS2C::GP_SERV_COMMAND_REQSUBMAPNUM, 0x10E, "GP_SERV_COMMAND_REQSUBMAPNUM" },
        { PacketS2C::GP_SERV_COMMAND_EMOTE_LIST, 0x11A, "GP_SERV_COMMAND_EMOTE_LIST" },
        { PacketS2C::GP_SERV_COMMAND_JUMP, 0x11E, "GP_SERV_COMMAND_JUMP" },
    });
}

auto testMagicEnumRangeAndContains() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint64_t>(magic_enum::customize::enum_range<PacketS2C>::min), 0, "PacketS2C magic_enum min") && ok;
    ok = expectEqualInt(static_cast<std::uint64_t>(magic_enum::customize::enum_range<PacketS2C>::max), 300, "PacketS2C magic_enum max") && ok;
    ok = expectEqualInt(static_cast<std::uint64_t>(magic_enum::enum_count<PacketS2C>()), 149, "PacketS2C magic_enum count") && ok;

    ok = expectTrue(magic_enum::enum_contains(PacketS2C::GP_SERV_COMMAND_PACKETCONTROL), "magic_enum contains PACKETCONTROL") && ok;
    ok = expectTrue(magic_enum::enum_contains(static_cast<PacketS2C>(0x08E)), "magic_enum contains ALTER_EGO_POINTS by value") && ok;
    ok = expectTrue(magic_enum::enum_contains(PacketS2C::GP_SERV_COMMAND_JUMP), "magic_enum contains JUMP") && ok;

    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketS2C>(0x000)), "magic_enum excludes range-min hole") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketS2C>(0x007)), "magic_enum excludes sparse hole 0x007") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketS2C>(0x024)), "magic_enum excludes sparse hole 0x024") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketS2C>(0x064)), "magic_enum excludes sparse hole 0x064") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketS2C>(0x11B)), "magic_enum excludes sparse hole 0x11B") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketS2C>(300)), "magic_enum excludes range-max hole") && ok;
    ok = expectFalse(magic_enum::enum_contains(static_cast<PacketS2C>(301)), "magic_enum excludes value beyond customized range") && ok;

    return ok;
}

} // namespace

auto runS2CPacketIDEnumSelfTests() -> bool
{
    bool ok = true;

    ok = testRepresentativePacketIDValues() && ok;
    ok = testMagicEnumRangeAndContains() && ok;

    return ok;
}
