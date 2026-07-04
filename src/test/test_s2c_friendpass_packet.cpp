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

#include "test_s2c_friendpass_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x059_friendpass.h"

namespace
{

constexpr auto friendPassLeftNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRIENDPASS::PacketData, leftNum);
constexpr auto friendPassLeftDaysOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRIENDPASS::PacketData, leftDays);
constexpr auto friendPassPassPopOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRIENDPASS::PacketData, passPop);
constexpr auto friendPassStringOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRIENDPASS::PacketData, String);
constexpr auto friendPassTypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRIENDPASS::PacketData, Type);
constexpr auto friendPassUnknown21Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRIENDPASS::PacketData, unknown21);
constexpr auto friendPassPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRIENDPASS::PacketData, padding00);
constexpr auto friendPassPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_FRIENDPASS::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c FRIENDPASS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c FRIENDPASS packet self-test failed: " << label << " got";
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

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < end; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c FRIENDPASS packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_FRIENDPASS::PacketData), 32, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(friendPassPacketSize, 36, "packet size") && ok;
    ok      = expectEqualUInt(friendPassLeftNumOffset, 4, "leftNum offset") && ok;
    ok      = expectEqualUInt(friendPassLeftDaysOffset, 8, "leftDays offset") && ok;
    ok      = expectEqualUInt(friendPassPassPopOffset, 12, "passPop offset") && ok;
    ok      = expectEqualUInt(friendPassStringOffset, 16, "String offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_FRIENDPASS::PacketData::String), 16, "String size") && ok;
    ok      = expectEqualUInt(friendPassTypeOffset, 32, "Type offset") && ok;
    ok      = expectEqualUInt(friendPassUnknown21Offset, 33, "unknown21 offset") && ok;
    ok      = expectEqualUInt(friendPassPadding00Offset, 34, "padding00 offset") && ok;
    return ok;
}

auto testEmptyWorldPassConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_FRIENDPASS(0);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 36>{
        0x59, 0x12, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x10, 0x27, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x03, 0x01, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x059, "empty type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "empty size") && ok;
    ok      = expectBytes(packet, 0, expected, "empty encoded packet") && ok;
    ok      = expectZeroRange(packet, expected.size(), PACKET_SIZE, "empty tail") && ok;
    return ok;
}

auto testWorldPassConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_FRIENDPASS(12345);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 36>{
        0x59, 0x12, 0xEF, 0xBE,
        0x01, 0x00, 0x00, 0x00,
        0xA7, 0x00, 0x00, 0x00,
        0x10, 0x27, 0x00, 0x00,
        '0', '0', '0', '0',
        '0', '1', '2', '3',
        '4', '5', 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x06, 0x01, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x059, "world-pass type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "world-pass size") && ok;
    ok      = expectBytes(packet, 0, expected, "world-pass encoded packet") && ok;
    ok      = expectZeroRange(packet, expected.size(), PACKET_SIZE, "world-pass tail") && ok;
    return ok;
}

} // namespace

auto runS2CFriendPassPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEmptyWorldPassConstructor() && ok;
    ok      = testWorldPassConstructor() && ok;
    return ok;
}
