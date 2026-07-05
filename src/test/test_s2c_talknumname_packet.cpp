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

#include "test_s2c_talknumname_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "entities/char_entity.h"
#include "map/packets/s2c/0x043_talknumname.h"

namespace
{

constexpr auto talkNumNameUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMNAME::PacketData, UniqueNo);
constexpr auto talkNumNameActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMNAME::PacketData, ActIndex);
constexpr auto talkNumNameMesNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMNAME::PacketData, MesNum);
constexpr auto talkNumNameTypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMNAME::PacketData, Type);
constexpr auto talkNumNamePadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMNAME::PacketData, padding00);
constexpr auto talkNumNamePadding01Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMNAME::PacketData, padding01);
constexpr auto talkNumNameSNameOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMNAME::PacketData, sName);
constexpr auto talkNumNamePacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_TALKNUMNAME::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void makeChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid, std::string name)
{
    character.id    = id;
    character.targid = targid;
    character.name   = std::move(name);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUMNAME packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUMNAME packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TALKNUMNAME packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TALKNUMNAME packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto nameBytes(std::string_view name) -> std::array<uint8, 16>
{
    auto bytes = std::array<uint8, 16>{};
    std::memcpy(bytes.data(), name.data(), std::min<std::size_t>(name.size(), bytes.size()));
    return bytes;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TALKNUMNAME::PacketData::sName), 16, "sName size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TALKNUMNAME::PacketData), 28, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(talkNumNamePacketSize, 32, "packet size") && ok;
    ok      = expectEqualUInt(talkNumNameUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(talkNumNameActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(talkNumNameMesNumOffset, 10, "MesNum offset") && ok;
    ok      = expectEqualUInt(talkNumNameTypeOffset, 12, "Type offset") && ok;
    ok      = expectEqualUInt(talkNumNamePadding00Offset, 13, "padding00 offset") && ok;
    ok      = expectEqualUInt(talkNumNamePadding01Offset, 14, "padding01 offset") && ok;
    ok      = expectEqualUInt(talkNumNameSNameOffset, 16, "sName offset") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_SERV_COMMAND_TALKNUMNAME_TYPE::Unknown0), 0, "Unknown0 enum") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_SERV_COMMAND_TALKNUMNAME_TYPE::Unknown7), 7, "Unknown7 enum") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, "Alice");
    auto packet = GP_SERV_COMMAND_TALKNUMNAME(&character, 0x1234);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x043, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), talkNumNamePacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x43, 0x10, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, talkNumNameUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNo") && ok;
    ok      = expectBytes(packet, talkNumNameActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndex") && ok;
    ok      = expectBytes(packet, talkNumNameMesNumOffset, std::array<uint8, 2>{ 0x34, 0x92 }, "MesNum") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumNameTypeOffset], 0, "Type") && ok;
    ok      = expectZeroRange(packet, talkNumNamePadding00Offset, talkNumNameSNameOffset, "padding") && ok;
    ok      = expectBytes(packet, talkNumNameSNameOffset, nameBytes("Alice"), "sName") && ok;
    ok      = expectZeroRange(packet, talkNumNamePacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testEmbeddedNulNameCopiesRawBytes() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x01020304, 0x0506, std::string("Bob\0Raw", 7));
    auto packet = GP_SERV_COMMAND_TALKNUMNAME(&character, 0x0142);

    bool ok = true;
    ok      = expectBytes(packet, talkNumNameSNameOffset, nameBytes(std::string_view("Bob\0Raw", 7)), "embedded-nul sName") && ok;
    ok      = expectZeroRange(packet, talkNumNamePadding00Offset, talkNumNameSNameOffset, "embedded padding") && ok;
    return ok;
}

auto testLongNameTruncatesWithoutTerminator() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x01020304, 0x0506, "abcdefghijklmnopZ");
    auto packet = GP_SERV_COMMAND_TALKNUMNAME(&character, 0x0142);

    bool ok = true;
    ok      = expectBytes(packet, talkNumNameSNameOffset, nameBytes("abcdefghijklmnop"), "long sName") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumNameSNameOffset + 15], 'p', "long sName[15]") && ok;
    return ok;
}

auto testHighMessageIDWraps() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x01020304, 0x0506, "");
    auto packet = GP_SERV_COMMAND_TALKNUMNAME(&character, 0x9001);

    bool ok = true;
    ok      = expectBytes(packet, talkNumNameMesNumOffset, std::array<uint8, 2>{ 0x01, 0x10 }, "wrapped MesNum") && ok;
    ok      = expectBytes(packet, talkNumNameSNameOffset, nameBytes(""), "empty sName") && ok;
    return ok;
}

} // namespace

auto runS2CTalkNumNamePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    ok      = testEmbeddedNulNameCopiesRawBytes() && ok;
    ok      = testLongNameTruncatesWithoutTerminator() && ok;
    ok      = testHighMessageIDWraps() && ok;
    return ok;
}
