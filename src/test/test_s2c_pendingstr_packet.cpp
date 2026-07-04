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

#include "test_s2c_pendingstr_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/s2c/0x05d_pendingstr.h"

namespace
{

constexpr auto pendingStrNumOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PENDINGSTR::PacketData, num);
constexpr auto pendingStrString1Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PENDINGSTR::PacketData, string1);
constexpr auto pendingStrString2Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PENDINGSTR::PacketData, string2);
constexpr auto pendingStrString3Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PENDINGSTR::PacketData, string3);
constexpr auto pendingStrString4Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PENDINGSTR::PacketData, string4);
constexpr auto pendingStrPacketSize        = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_PENDINGSTR::PacketData);
constexpr auto pendingStrNumElementSize    = sizeof(GP_SERV_COMMAND_PENDINGSTR::PacketData::num[0]);
constexpr auto pendingStrStringElementSize = sizeof(GP_SERV_COMMAND_PENDINGSTR::PacketData::string1);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c PENDINGSTR packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c PENDINGSTR packet self-test failed: " << label << " got";
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
            std::cerr << "s2c PENDINGSTR packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto stringBytes(std::string_view text) -> std::array<uint8, 16>
{
    auto bytes = std::array<uint8, 16>{};
    std::memcpy(bytes.data(), text.data(), std::min<std::size_t>(text.size(), bytes.size() - 1));
    return bytes;
}

auto numOffset(std::size_t index) -> std::size_t
{
    return pendingStrNumOffset + index * pendingStrNumElementSize;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_PENDINGSTR::PacketData::num) / pendingStrNumElementSize, 9, "num element count") && ok;
    ok      = expectEqualUInt(pendingStrNumElementSize, 4, "num element size") && ok;
    ok      = expectEqualUInt(pendingStrStringElementSize, 16, "string element size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_PENDINGSTR::PacketData), 100, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(pendingStrPacketSize, 104, "packet size") && ok;
    ok      = expectEqualUInt(pendingStrNumOffset, 4, "num offset") && ok;
    ok      = expectEqualUInt(pendingStrString1Offset, 40, "string1 offset") && ok;
    ok      = expectEqualUInt(pendingStrString2Offset, 56, "string2 offset") && ok;
    ok      = expectEqualUInt(pendingStrString3Offset, 72, "string3 offset") && ok;
    ok      = expectEqualUInt(pendingStrString4Offset, 88, "string4 offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_PENDINGSTR("one", "two", "three", "four", 0x11223344, 2, 3, 4, 5, 6, 7, 8, 0xFFFFFFFF);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x05D, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), pendingStrPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x5D, 0x34, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, numOffset(0), std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "num[0]") && ok;
    ok      = expectBytes(packet, numOffset(8), std::array<uint8, 4>{ 0xFF, 0xFF, 0xFF, 0xFF }, "num[8]") && ok;
    ok      = expectBytes(packet, pendingStrString1Offset, stringBytes("one"), "string1") && ok;
    ok      = expectBytes(packet, pendingStrString2Offset, stringBytes("two"), "string2") && ok;
    ok      = expectBytes(packet, pendingStrString3Offset, stringBytes("three"), "string3") && ok;
    ok      = expectBytes(packet, pendingStrString4Offset, stringBytes("four"), "string4") && ok;
    ok      = expectZeroRange(packet, pendingStrPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testEmbeddedNulStringsCopyRawBytes() -> bool
{
    auto packet = GP_SERV_COMMAND_PENDINGSTR(std::string("ab\0cd", 5), std::string("ef\0gh", 5), "", "");

    bool ok = true;
    ok      = expectBytes(packet, pendingStrString1Offset, stringBytes(std::string_view("ab\0cd", 5)), "embedded string1") && ok;
    ok      = expectBytes(packet, pendingStrString2Offset, stringBytes(std::string_view("ef\0gh", 5)), "embedded string2") && ok;
    ok      = expectBytes(packet, pendingStrString3Offset, stringBytes(""), "empty string3") && ok;
    ok      = expectBytes(packet, pendingStrString4Offset, stringBytes(""), "empty string4") && ok;
    return ok;
}

auto testLongStringsTruncateAtFifteenBytes() -> bool
{
    auto packet = GP_SERV_COMMAND_PENDINGSTR("abcdefghijklmnop", "ABCDEFGHIJKLMNOZ", "1234567890123456", "zyxwvutsrqponmlk");

    bool ok = true;
    ok      = expectBytes(packet, pendingStrString1Offset, stringBytes("abcdefghijklmno"), "long string1") && ok;
    ok      = expectBytes(packet, pendingStrString2Offset, stringBytes("ABCDEFGHIJKLMNO"), "long string2") && ok;
    ok      = expectBytes(packet, pendingStrString3Offset, stringBytes("123456789012345"), "long string3") && ok;
    ok      = expectBytes(packet, pendingStrString4Offset, stringBytes("zyxwvutsrqponml"), "long string4") && ok;
    ok      = expectEqualUInt(packetData(packet)[pendingStrString1Offset + 15], 0, "long string1[15]") && ok;
    return ok;
}

auto testDefaultConstructorZeroesPayload() -> bool
{
    auto packet = GP_SERV_COMMAND_PENDINGSTR();

    bool ok = true;
    ok      = expectZeroRange(packet, pendingStrNumOffset, pendingStrPacketSize, "default payload") && ok;
    ok      = expectZeroRange(packet, pendingStrPacketSize, PACKET_SIZE, "default tail") && ok;
    return ok;
}

} // namespace

auto runS2CPendingStrPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    ok      = testEmbeddedNulStringsCopyRawBytes() && ok;
    ok      = testLongStringsTruncateAtFifteenBytes() && ok;
    ok      = testDefaultConstructorZeroesPayload() && ok;
    return ok;
}
