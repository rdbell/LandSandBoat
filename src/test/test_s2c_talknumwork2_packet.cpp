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

#include "test_s2c_talknumwork2_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

#include "entities/char_entity.h"
#include "map/packets/s2c/0x027_talknumwork2.h"

namespace
{

constexpr auto talkNumWork2UniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, UniqueNo);
constexpr auto talkNumWork2ActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, ActIndex);
constexpr auto talkNumWork2MesNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, MesNum);
constexpr auto talkNumWork2TypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, Type);
constexpr auto talkNumWork2FlagsOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, Flags);
constexpr auto talkNumWork2Padding0FOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, padding0F);
constexpr auto talkNumWork2Num1Offset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, Num1);
constexpr auto talkNumWork2String1Offset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, String1);
constexpr auto talkNumWork2String2Offset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, String2);
constexpr auto talkNumWork2Num2Offset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData, Num2);
constexpr auto talkNumWork2PacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData);
constexpr auto talkNumWork2Num1Count       = sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData::Num1) / sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData::Num1[0]);
constexpr auto talkNumWork2Num2Count       = sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData::Num2) / sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData::Num2[0]);
constexpr auto talkNumWork2NumElementSize  = sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData::Num1[0]);
constexpr auto talkNumWork2String1Len      = sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData::String1);
constexpr auto talkNumWork2String2Len      = sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData::String2);

void makeChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid, std::string name)
{
    character.id    = id;
    character.targid = targid;
    character.name   = std::move(name);
}

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUMWORK2 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUMWORK2 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TALKNUMWORK2 packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TALKNUMWORK2 packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto string1Bytes(std::string_view name) -> std::array<uint8, talkNumWork2String1Len>
{
    auto bytes = std::array<uint8, talkNumWork2String1Len>{};
    std::memcpy(bytes.data(), name.data(), std::min<std::size_t>(name.size(), bytes.size()));
    return bytes;
}

auto num1Offset(std::size_t index) -> std::size_t
{
    return talkNumWork2Num1Offset + index * talkNumWork2NumElementSize;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(talkNumWork2Num1Count, 4, "Num1 count") && ok;
    ok      = expectEqualUInt(talkNumWork2Num2Count, 8, "Num2 count") && ok;
    ok      = expectEqualUInt(talkNumWork2NumElementSize, 4, "Num element size") && ok;
    ok      = expectEqualUInt(talkNumWork2String1Len, 32, "String1 size") && ok;
    ok      = expectEqualUInt(talkNumWork2String2Len, 16, "String2 size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TALKNUMWORK2::PacketData), 108, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(talkNumWork2PacketSize, 112, "packet size") && ok;
    ok      = expectEqualUInt(talkNumWork2UniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(talkNumWork2ActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(talkNumWork2MesNumOffset, 10, "MesNum offset") && ok;
    ok      = expectEqualUInt(talkNumWork2TypeOffset, 12, "Type offset") && ok;
    ok      = expectEqualUInt(talkNumWork2FlagsOffset, 14, "Flags offset") && ok;
    ok      = expectEqualUInt(talkNumWork2Padding0FOffset, 15, "padding0F offset") && ok;
    ok      = expectEqualUInt(talkNumWork2Num1Offset, 16, "Num1 offset") && ok;
    ok      = expectEqualUInt(talkNumWork2String1Offset, 32, "String1 offset") && ok;
    ok      = expectEqualUInt(talkNumWork2String2Offset, 64, "String2 offset") && ok;
    ok      = expectEqualUInt(talkNumWork2Num2Offset, 80, "Num2 offset") && ok;
    return ok;
}

auto testFishingConstructorCopiesFullStringField() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, std::string("ab\0cdefghijklmnopqrstuvwxyz012345", 33));
    auto packet = GP_SERV_COMMAND_TALKNUMWORK2(&character, 0x789A, 0x1234, 0xEF);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x027, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), talkNumWork2PacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x27, 0x38, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, talkNumWork2UniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNo") && ok;
    ok      = expectBytes(packet, talkNumWork2ActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndex") && ok;
    ok      = expectBytes(packet, talkNumWork2MesNumOffset, std::array<uint8, 2>{ 0x34, 0x92 }, "MesNum") && ok;
    ok      = expectBytes(packet, talkNumWork2TypeOffset, std::array<uint8, 2>{ 0, 0 }, "Type") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumWork2FlagsOffset], 0, "Flags") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumWork2Padding0FOffset], 0, "padding0F") && ok;
    ok      = expectBytes(packet, num1Offset(0), std::array<uint8, 4>{ 0x9A, 0x78, 0x00, 0x00 }, "Num1[0]") && ok;
    ok      = expectBytes(packet, num1Offset(1), std::array<uint8, 4>{ 0xEF, 0x00, 0x00, 0x00 }, "Num1[1]") && ok;
    ok      = expectBytes(packet, talkNumWork2String1Offset, string1Bytes(std::string_view("ab\0cdefghijklmnopqrstuvwxyz01234", 32)), "String1") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumWork2String1Offset + 31], static_cast<uint8>('4'), "String1[31]") && ok;
    ok      = expectZeroRange(packet, talkNumWork2String2Offset, talkNumWork2PacketSize, "String2 and Num2") && ok;
    ok      = expectZeroRange(packet, talkNumWork2PacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testMessageConstructorNameActorAndShowSender() -> bool
{
    auto actor     = CCharEntity{};
    auto nameActor = CCharEntity{};
    makeChar(actor, 0x01020304, 0x0708, "Actor");
    makeChar(nameActor, 0xAABBCCDD, 0xEEFF, "SenderName");
    auto packet = GP_SERV_COMMAND_TALKNUMWORK2(&actor, 0x9001, &nameActor, -1, 0x7FFFFFFF, static_cast<int32>(0x80000000), static_cast<int32>(0xA0B0C0D0), 0x1234, true);

    bool ok = true;
    ok      = expectBytes(packet, talkNumWork2UniqueNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "message UniqueNo") && ok;
    ok      = expectBytes(packet, talkNumWork2ActIndexOffset, std::array<uint8, 2>{ 0x08, 0x07 }, "message ActIndex") && ok;
    ok      = expectBytes(packet, talkNumWork2MesNumOffset, std::array<uint8, 2>{ 0x01, 0x90 }, "message MesNum") && ok;
    ok      = expectBytes(packet, talkNumWork2TypeOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "message Type") && ok;
    ok      = expectBytes(packet, num1Offset(0), std::array<uint8, 4>{ 0xFF, 0xFF, 0xFF, 0xFF }, "message Num1[0]") && ok;
    ok      = expectBytes(packet, num1Offset(1), std::array<uint8, 4>{ 0xFF, 0xFF, 0xFF, 0x7F }, "message Num1[1]") && ok;
    ok      = expectBytes(packet, num1Offset(2), std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x80 }, "message Num1[2]") && ok;
    ok      = expectBytes(packet, num1Offset(3), std::array<uint8, 4>{ 0xD0, 0xC0, 0xB0, 0xA0 }, "message Num1[3]") && ok;
    ok      = expectBytes(packet, talkNumWork2String1Offset, string1Bytes("SenderName"), "message String1") && ok;
    ok      = expectZeroRange(packet, talkNumWork2String2Offset, talkNumWork2PacketSize, "message String2 and Num2") && ok;
    return ok;
}

auto testMessageConstructorDefaultsHideSenderAndUseActorName() -> bool
{
    auto actor = CCharEntity{};
    makeChar(actor, 0x01020304, 0x0708, "ActorName");
    auto packet = GP_SERV_COMMAND_TALKNUMWORK2(&actor, 0x0142);

    bool ok = true;
    ok      = expectBytes(packet, talkNumWork2MesNumOffset, std::array<uint8, 2>{ 0x42, 0x81 }, "hidden MesNum") && ok;
    ok      = expectBytes(packet, talkNumWork2TypeOffset, std::array<uint8, 2>{ 0x04, 0x00 }, "default Type") && ok;
    ok      = expectBytes(packet, talkNumWork2String1Offset, string1Bytes("ActorName"), "default String1") && ok;
    ok      = expectZeroRange(packet, talkNumWork2String2Offset, talkNumWork2PacketSize, "default String2 and Num2") && ok;
    return ok;
}

} // namespace

auto runS2CTalkNumWork2PacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testFishingConstructorCopiesFullStringField() && ok;
    ok      = testMessageConstructorNameActorAndShowSender() && ok;
    ok      = testMessageConstructorDefaultsHideSenderAndUseActorName() && ok;
    return ok;
}
