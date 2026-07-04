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

#include "test_c2s_packet_base_helpers.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/basic.h"
#include "map/packets/c2s/base.h"

namespace
{

GP_CLI_PACKET(GP_CLI_COMMAND_GAMEOK,
    std::uint32_t accountId;
    std::uint16_t zoneId;
    std::uint8_t flags[2];
);

GP_CLI_PACKET(GP_CLI_COMMAND_CLSTAT,
    std::uint8_t flag;
);

GP_CLI_PACKET_VLA(GP_CLI_COMMAND_GM, Command,
    std::uint16_t flags;
    std::uint8_t Command[1];
);

using HeaderBytes = std::array<std::uint8_t, sizeof(GP_CLI_HEADER)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s packet base helper self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s packet base helper self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s packet base helper self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(const HeaderBytes& actual, const HeaderBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s packet base helper self-test failed: " << label << " got";
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

auto testHeaderLayout() -> bool
{
    auto header = GP_CLI_HEADER{};
    header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ROE_CLAIM);
    header.size = 4;
    header.sync = 0xBEEF;

    auto headerBytes = HeaderBytes{};
    std::memcpy(headerBytes.data(), &header, headerBytes.size());

    auto basic = CBasicPacket{};
    basic.setType(static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ROE_CLAIM));
    basic.setSize(16);
    basic.setSequence(0xBEEF);

    auto basicBytes = HeaderBytes{};
    std::memcpy(basicBytes.data(), static_cast<uint8*>(basic), basicBytes.size());

    const auto expectedBytes = HeaderBytes{ 0x0E, 0x09, 0xEF, 0xBE };

    bool ok = true;
    ok      = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok      = expectEqualInt(header.id, 0x10E, "GP_CLI_HEADER id bitfield") && ok;
    ok      = expectEqualInt(header.size, 4, "GP_CLI_HEADER size bitfield") && ok;
    ok      = expectEqualInt(header.sync, 0xBEEF, "GP_CLI_HEADER sync") && ok;
    ok      = expectBytes(headerBytes, expectedBytes, "GP_CLI_HEADER encoded bytes") && ok;
    ok      = expectBytes(basicBytes, expectedBytes, "CBasicPacket-compatible encoded bytes") && ok;

    return ok;
}

auto testFixedPacketDeclaration() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_GAMEOK::name, "GP_CLI_COMMAND_GAMEOK", "fixed packet static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_GAMEOK::packetId == PacketC2S::GP_CLI_COMMAND_GAMEOK, "fixed packet static packetId") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GAMEOK, header), 0, "fixed packet header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GAMEOK, accountId), sizeof(GP_CLI_HEADER), "fixed packet first payload offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GAMEOK), 12, "fixed packet sizeof") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_GAMEOK))), 12, "fixed packet rounded dispatch size") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_CLSTAT::name, "GP_CLI_COMMAND_CLSTAT", "unaligned fixed packet static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_CLSTAT::packetId == PacketC2S::GP_CLI_COMMAND_CLSTAT, "unaligned fixed packet static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CLSTAT), 6, "unaligned fixed packet sizeof") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_CLSTAT))), 8, "unaligned fixed rounded dispatch size") && ok;

    return ok;
}

auto testVLAPacketDeclaration() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_GM::name, "GP_CLI_COMMAND_GM", "VLA packet static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_GM::packetId == PacketC2S::GP_CLI_COMMAND_GM, "VLA packet static packetId") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GM, header), 0, "VLA packet header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GM, Command), sizeof(GP_CLI_HEADER) + sizeof(std::uint16_t), "VLA field offset") && ok;
    ok = expectEqualInt(GP_CLI_COMMAND_GM::getMinSize(), offsetof(GP_CLI_COMMAND_GM, Command), "VLA getMinSize") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GM), 8, "VLA packet sizeof") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_GM))), 8, "VLA rounded dispatch size") && ok;

    return ok;
}

} // namespace

auto runC2SPacketBaseHelperSelfTests() -> bool
{
    bool ok = true;

    ok = testHeaderLayout() && ok;
    ok = testFixedPacketDeclaration() && ok;
    ok = testVLAPacketDeclaration() && ok;

    return ok;
}
