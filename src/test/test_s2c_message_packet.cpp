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

#include "test_s2c_message_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/enums/msg_std.h"
#include "map/packets/s2c/0x009_message.h"

namespace
{

constexpr auto messageUniqueNoOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MESSAGE::PacketData, UniqueNo);
constexpr auto messageActIndexOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MESSAGE::PacketData, ActIndex);
constexpr auto messageMesNoOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MESSAGE::PacketData, MesNo);
constexpr auto messageAttrOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MESSAGE::PacketData, Attr);
constexpr auto messageDataOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MESSAGE::PacketData, Data);
constexpr auto messagePacketDefaultSize    = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_MESSAGE::PacketData);
constexpr auto messageIDPacketSize         = 0x10;
constexpr auto messageParamPacketSize      = 0x1C;
constexpr auto messageParams2PacketSize    = 0x48;
constexpr auto messageNoCharParams2Size    = 0x24;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MESSAGE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c MESSAGE packet self-test failed: " << label << " got";
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

auto expectData(CBasicPacket& packet, const std::string& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + messageDataOffset;
    if (std::strncmp(reinterpret_cast<const char*>(data), expected.c_str(), expected.size() + 1) != 0)
    {
        std::cerr << "s2c MESSAGE packet self-test failed: " << label << " got \""
                  << reinterpret_cast<const char*>(data) << "\" expected \"" << expected << "\"\n";
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
            std::cerr << "s2c MESSAGE packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
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
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_MESSAGE::PacketData), 140, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(messagePacketDefaultSize, 144, "packet default size") && ok;
    ok      = expectEqualUInt(messageUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(messageActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(messageMesNoOffset, 10, "MesNo offset") && ok;
    ok      = expectEqualUInt(messageAttrOffset, 12, "Attr offset") && ok;
    ok      = expectEqualUInt(messageDataOffset, 13, "Data offset") && ok;
    return ok;
}

auto testMessageIDConstructors() -> bool
{
    auto packet = GP_SERV_COMMAND_MESSAGE(MsgStd::EventSkipped);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, messageIDPacketSize>{
        0x09, 0x08, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x75, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    auto rawPacket = GP_SERV_COMMAND_MESSAGE(static_cast<uint16>(0xBEEF));

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x009, "MESSAGE type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "MsgStd constructor size") && ok;
    ok      = expectBytes(packet, expected, "encoded MsgStd constructor") && ok;
    ok      = expectZeroTail(packet, expected.size(), "MsgStd constructor tail") && ok;
    ok      = expectEqualUInt(rawPacket.getSize(), messageIDPacketSize, "raw constructor size") && ok;
    ok      = expectEqualUInt(packetData(rawPacket)[messageMesNoOffset], 0xEF, "raw MesNo low byte") && ok;
    ok      = expectEqualUInt(packetData(rawPacket)[messageMesNoOffset + 1], 0xBE, "raw MesNo high byte") && ok;
    return ok;
}

auto testParamConstructors() -> bool
{
    auto param0Packet = GP_SERV_COMMAND_MESSAGE(123U, MsgStd::BlockedByBlockaid);
    param0Packet.setSequence(0xBEEF);

    const auto expectedParam0Prefix = std::array<uint8, 13>{
        0x09, 0x0E, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xE2, 0x00,
        0x00,
    };

    auto string2Packet = GP_SERV_COMMAND_MESSAGE(std::string("abcdefghijklmnop"), MsgStd::PollProposalSay);
    auto nulPacket     = GP_SERV_COMMAND_MESSAGE(std::string("ab\0cd", 5), MsgStd::PollProposalSay);
    auto params2Packet = GP_SERV_COMMAND_MESSAGE(11U, 22U, static_cast<uint16>(0x1234));
    auto noCharPacket  = GP_SERV_COMMAND_MESSAGE(nullptr, 33U, 44U, MsgStd::CannotInvite);

    bool ok = true;
    ok      = expectEqualUInt(param0Packet.getSize(), messageParamPacketSize, "param0 constructor size") && ok;
    ok      = expectBytes(param0Packet, expectedParam0Prefix, "encoded param0 prefix") && ok;
    ok      = expectData(param0Packet, "Para0 123 ", "param0 Data") && ok;
    ok      = expectZeroTail(param0Packet, messageParamPacketSize, "param0 tail") && ok;

    ok = expectEqualUInt(string2Packet.getSize(), messageParamPacketSize, "string2 constructor size") && ok;
    ok = expectData(string2Packet, "string2 abcdefghijklmno", "string2 truncated Data") && ok;

    ok = expectEqualUInt(nulPacket.getSize(), messageParamPacketSize, "string2 embedded NUL constructor size") && ok;
    ok = expectData(nulPacket, "string2 ab", "string2 embedded NUL Data") && ok;
    ok = expectEqualUInt(packetData(nulPacket)[messageDataOffset + std::strlen("string2 ab") + 1], 0, "string2 embedded NUL byte after terminator") && ok;

    ok = expectEqualUInt(params2Packet.getSize(), messageParams2PacketSize, "params2 constructor size") && ok;
    ok = expectData(params2Packet, "Para0 11 Para1 22", "params2 Data") && ok;
    ok = expectEqualUInt(packetData(params2Packet)[messageMesNoOffset], 0x34, "params2 MesNo low byte") && ok;
    ok = expectEqualUInt(packetData(params2Packet)[messageMesNoOffset + 1], 0x12, "params2 MesNo high byte") && ok;

    ok = expectEqualUInt(noCharPacket.getSize(), messageNoCharParams2Size, "null character branch size") && ok;
    ok = expectData(noCharPacket, "Para0 33 Para1 44", "null character branch Data") && ok;
    return ok;
}

auto testParams4Constructor() -> bool
{
    auto packet = GP_SERV_COMMAND_MESSAGE(1U, 2U, 3U, 4U, MsgStd::Compass);
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 13>{
        0x09, 0x10, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xEF, 0x00,
        0x00,
    };

    bool ok = true;
    ok      = expectBytes(packet, expectedPrefix, "encoded params4 prefix") && ok;
    ok      = expectData(packet, "Para0 1 Para1 2 Para2 3 Para3 4", "params4 Data") && ok;
    ok      = expectEqualUInt(packet.getSize(), 32, "params4 declared size") && ok;
    ok      = expectEqualUInt(packetData(packet)[packet.getSize()], static_cast<uint8>('a'), "params4 data past declared size") && ok;
    return ok;
}

} // namespace

auto runS2CMessagePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testMessageIDConstructors() && ok;
    ok      = testParamConstructors() && ok;
    ok      = testParams4Constructor() && ok;
    return ok;
}
