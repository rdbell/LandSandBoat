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

#include "test_s2c_myroom_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x096_myroom_enter.h"
#include "map/packets/s2c/0x097_myroom_exit.h"

namespace
{

constexpr auto myRoomEnterResultOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_ENTER::PacketData, Result);
constexpr auto myRoomEnterPadding05Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_ENTER::PacketData, padding05);
constexpr auto myRoomEnterPacketDefaultSize  = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_MYROOM_ENTER::PacketData);
constexpr auto myRoomExitResultOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_EXIT::PacketData, Result);
constexpr auto myRoomExitPadding05Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_EXIT::PacketData, padding05);
constexpr auto myRoomExitPacketDefaultSize   = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_MYROOM_EXIT::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MyRoom packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(CBasicPacket& packet, const std::array<uint8, N>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c MyRoom packet self-test failed: " << label << " got";
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
            std::cerr << "s2c MyRoom packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testMyRoomEnterLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_MYROOM_ENTER::PacketData), 4, "MYROOM_ENTER sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(myRoomEnterPacketDefaultSize, 8, "MYROOM_ENTER default size") && ok;
    ok      = expectEqualUInt(myRoomEnterResultOffset, 4, "MYROOM_ENTER Result offset") && ok;
    ok      = expectEqualUInt(myRoomEnterPadding05Offset, 5, "MYROOM_ENTER padding05 offset") && ok;
    return ok;
}

auto testMyRoomExitLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_MYROOM_EXIT::PacketData), 4, "MYROOM_EXIT sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(myRoomExitPacketDefaultSize, 8, "MYROOM_EXIT default size") && ok;
    ok      = expectEqualUInt(myRoomExitResultOffset, 4, "MYROOM_EXIT Result offset") && ok;
    ok      = expectEqualUInt(myRoomExitPadding05Offset, 5, "MYROOM_EXIT padding05 offset") && ok;
    return ok;
}

auto testMyRoomEnterConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_MYROOM_ENTER();
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 8>{
        0x96, 0x04, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x096, "MYROOM_ENTER type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "MYROOM_ENTER size") && ok;
    ok      = expectBytes(packet, expected, "encoded MYROOM_ENTER prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "MYROOM_ENTER tail") && ok;
    return ok;
}

auto testMyRoomExitConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_MYROOM_EXIT();
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 8>{
        0x97, 0x04, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x097, "MYROOM_EXIT type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "MYROOM_EXIT size") && ok;
    ok      = expectBytes(packet, expected, "encoded MYROOM_EXIT prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "MYROOM_EXIT tail") && ok;
    return ok;
}

} // namespace

auto runS2CMyRoomPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testMyRoomEnterLayout() && ok;
    ok      = testMyRoomExitLayout() && ok;
    ok      = testMyRoomEnterConstructor() && ok;
    ok      = testMyRoomExitConstructor() && ok;
    return ok;
}
