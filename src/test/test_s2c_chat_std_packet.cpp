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

#include "test_s2c_chat_std_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/ipc_structs.h"
#include "map/enums/chat_message_type.h"
#include "map/packets/s2c/0x017_chat_std.h"

namespace
{

constexpr auto chatStdKindOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_CHAT_STD::PacketData, Kind);
constexpr auto chatStdAttrOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_CHAT_STD::PacketData, Attr);
constexpr auto chatStdDataOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_CHAT_STD::PacketData, Data);
constexpr auto chatStdSNameOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_CHAT_STD::PacketData, sName);
constexpr auto chatStdMesOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_CHAT_STD::PacketData, Mes);
constexpr auto chatStdPacketDataSize  = sizeof(GP_SERV_COMMAND_CHAT_STD::PacketData);
constexpr auto chatStdFullPacketSize  = sizeof(GP_SERV_HEADER) + chatStdPacketDataSize;
constexpr auto chatStdMaxMessageBytes = sizeof(GP_SERV_COMMAND_CHAT_STD::PacketData::Mes);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CHAT_STD packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CHAT_STD packet self-test failed: " << label << " got";
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

auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::string& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c CHAT_STD packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectRepeatedByte(CBasicPacket& packet, std::size_t offset, std::size_t count, uint8 expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = 0; i < count; ++i)
    {
        if (data[offset + i] != expected)
        {
            std::cerr << "s2c CHAT_STD packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[offset + i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto makeAlphabetBytes(std::size_t length) -> std::string
{
    auto value = std::string(length, '\0');
    for (std::size_t i = 0; i < value.size(); ++i)
    {
        value[i] = static_cast<char>('A' + (i % 26));
    }
    return value;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(chatStdPacketDataSize, 170, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(chatStdFullPacketSize, 174, "full packet size before dynamic rounding") && ok;
    ok      = expectEqualUInt(chatStdKindOffset, 4, "Kind offset") && ok;
    ok      = expectEqualUInt(chatStdAttrOffset, 5, "Attr offset") && ok;
    ok      = expectEqualUInt(chatStdDataOffset, 6, "Data offset") && ok;
    ok      = expectEqualUInt(chatStdSNameOffset, 8, "sName offset") && ok;
    ok      = expectEqualUInt(chatStdMesOffset, 23, "Mes offset") && ok;
    ok      = expectEqualUInt(chatStdMaxMessageBytes, 150, "Mes size") && ok;
    return ok;
}

auto testZoneConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_CHAT_STD("Alice", 0x1234, MESSAGE_YELL, "Hello", 0);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x017, "zone type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 28, "zone size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x17, 0x0E, 0xEF, 0xBE }, "zone header") && ok;
    ok      = expectBytes(packet, chatStdKindOffset, std::array<uint8, 4>{ 0x1A, 0x00, 0x34, 0x12 }, "zone scalar fields") && ok;
    ok      = expectBytes(packet, chatStdSNameOffset, std::array<uint8, 15>{ 'A', 'l', 'i', 'c', 'e' }, "zone sName") && ok;
    ok      = expectBytes(packet, chatStdMesOffset, std::array<uint8, 5>{ 'H', 'e', 'l', 'l', 'o' }, "zone Mes") && ok;
    ok      = expectRepeatedByte(packet, chatStdSNameOffset + 5, 10, 0, "zone sName padding") && ok;
    ok      = expectRepeatedByte(packet, chatStdMesOffset + 5, PACKET_SIZE - chatStdMesOffset - 5, 0, "zone message padding and tail") && ok;
    return ok;
}

auto testZoneGMBranch() -> bool
{
    auto packet = GP_SERV_COMMAND_CHAT_STD("", 0x0001, MESSAGE_SYSTEM_1, "", 3);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 24, "GM empty name size") && ok;
    ok      = expectBytes(packet, chatStdKindOffset, std::array<uint8, 4>{ 0x06, 0x01, 0x01, 0x00 }, "GM scalar fields") && ok;
    ok      = expectRepeatedByte(packet, chatStdSNameOffset, PACKET_SIZE - chatStdSNameOffset, 0, "GM empty name and message") && ok;
    return ok;
}

auto testAssistConstructor() -> bool
{
    auto payload        = ipc::ChatMessageAssist{};
    payload.senderName = "Mentor";
    payload.message    = "Help";
    payload.mentorRank = 2;
    payload.masteryRank = 7;
    payload.gmLevel    = 3;
    payload.messageType = MESSAGE_NA_ASSIST;

    auto packet = GP_SERV_COMMAND_CHAT_STD(payload);
    packet.setSequence(0x1234);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x017, "assist type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 28, "assist size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x17, 0x0E, 0x34, 0x12 }, "assist header") && ok;
    ok      = expectBytes(packet, chatStdKindOffset, std::array<uint8, 4>{ 0x23, 0x01, 0x07, 0x02 }, "assist scalar fields") && ok;
    ok      = expectBytes(packet, chatStdSNameOffset, std::array<uint8, 15>{ 'M', 'e', 'n', 't', 'o', 'r' }, "assist sName") && ok;
    ok      = expectBytes(packet, chatStdMesOffset, std::array<uint8, 4>{ 'H', 'e', 'l', 'p' }, "assist Mes") && ok;
    ok      = expectRepeatedByte(packet, chatStdSNameOffset + 6, 9, 0, "assist sName padding") && ok;
    ok      = expectRepeatedByte(packet, chatStdMesOffset + 4, PACKET_SIZE - chatStdMesOffset - 4, 0, "assist message padding and tail") && ok;
    return ok;
}

auto testTruncation() -> bool
{
    const auto name    = std::string("1234567890abcdef");
    const auto message = makeAlphabetBytes(chatStdMaxMessageBytes + 12);
    auto       packet  = GP_SERV_COMMAND_CHAT_STD(name, 0x2222, MESSAGE_SHOUT, message, 0);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 176, "truncated max message rounded size") && ok;
    ok      = expectBytes(packet, chatStdSNameOffset, std::array<uint8, 15>{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e' }, "truncated sName") && ok;
    ok      = expectBytes(packet, chatStdMesOffset, message.substr(0, chatStdMaxMessageBytes), "truncated Mes") && ok;
    ok      = expectRepeatedByte(packet, chatStdMesOffset + chatStdMaxMessageBytes, PACKET_SIZE - chatStdMesOffset - chatStdMaxMessageBytes, 0, "truncated message tail") && ok;
    return ok;
}

} // namespace

auto runS2CChatStdPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testZoneConstructor() && ok;
    ok      = testZoneGMBranch() && ok;
    ok      = testAssistConstructor() && ok;
    ok      = testTruncation() && ok;
    return ok;
}
