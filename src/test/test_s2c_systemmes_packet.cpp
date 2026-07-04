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

#include "test_s2c_systemmes_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/enums/msg_std.h"
#include "map/packets/s2c/0x053_systemmes.h"

namespace
{

constexpr auto systemMesParaOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SYSTEMMES::PacketData, para);
constexpr auto systemMesPara2Offset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SYSTEMMES::PacketData, para2);
constexpr auto systemMesNumberOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SYSTEMMES::PacketData, Number);
constexpr auto systemMesPadding0EOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SYSTEMMES::PacketData, padding0E);
constexpr auto systemMesPacketDefaultSize = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_SYSTEMMES::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c SYSTEMMES packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(CBasicPacket& packet, const std::array<uint8, 16>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c SYSTEMMES packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
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

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c SYSTEMMES packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(MsgStd), 2, "sizeof(MsgStd)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_SYSTEMMES::PacketData), 12, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(systemMesPacketDefaultSize, 16, "packet default size") && ok;
    ok      = expectEqualUInt(systemMesParaOffset, 4, "para offset") && ok;
    ok      = expectEqualUInt(systemMesPara2Offset, 8, "para2 offset") && ok;
    ok      = expectEqualUInt(systemMesNumberOffset, 12, "Number offset") && ok;
    ok      = expectEqualUInt(systemMesPadding0EOffset, 14, "padding0E offset") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(MsgStd::TargetIsCurrentlyBlocking), 225, "MsgStd::TargetIsCurrentlyBlocking") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_SYSTEMMES(0x11223344, 0x55667788, MsgStd::TargetIsCurrentlyBlocking);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 16>{
        0x53, 0x08, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x88, 0x77, 0x66, 0x55,
        0xE1, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x053, "SYSTEMMES type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "SYSTEMMES size") && ok;
    ok      = expectBytes(packet, expected, "encoded SYSTEMMES prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "SYSTEMMES tail") && ok;
    return ok;
}

} // namespace

auto runS2CSystemMesPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
