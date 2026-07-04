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

#include "test_s2c_translate_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/s2c/0x047_translate.h"

namespace
{

constexpr auto translateItemNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRANSLATE::PacketData, ItemNo);
constexpr auto translateFromIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRANSLATE::PacketData, FromIndex);
constexpr auto translateToIndexOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRANSLATE::PacketData, ToIndex);
constexpr auto translateFromStringOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRANSLATE::PacketData, FromString);
constexpr auto translateToStringOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRANSLATE::PacketData, ToString);
constexpr auto translatePacketSize       = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_TRANSLATE::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TRANSLATE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TRANSLATE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TRANSLATE packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TRANSLATE packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto stringBytes(std::string_view text) -> std::array<uint8, 64>
{
    auto bytes = std::array<uint8, 64>{};
    std::memcpy(bytes.data(), text.data(), std::min<std::size_t>(text.size(), bytes.size()));
    return bytes;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TRANSLATE::PacketData::FromString), 64, "FromString size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TRANSLATE::PacketData::ToString), 64, "ToString size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TRANSLATE::PacketData), 132, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(translatePacketSize, 136, "packet size") && ok;
    ok      = expectEqualUInt(translateItemNoOffset, 4, "ItemNo offset") && ok;
    ok      = expectEqualUInt(translateFromIndexOffset, 6, "FromIndex offset") && ok;
    ok      = expectEqualUInt(translateToIndexOffset, 7, "ToIndex offset") && ok;
    ok      = expectEqualUInt(translateFromStringOffset, 8, "FromString offset") && ok;
    ok      = expectEqualUInt(translateToStringOffset, 72, "ToString offset") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese), 0, "Japanese enum") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_TRANSLATE_INDEX::English), 1, "English enum") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_TRANSLATE(0x1234, GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese, GP_CLI_COMMAND_TRANSLATE_INDEX::English, "yagudo", "yagudo drink");
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x047, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), translatePacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x47, 0x44, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, translateItemNoOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "ItemNo") && ok;
    ok      = expectEqualInt(packetData(packet)[translateFromIndexOffset], 0, "FromIndex") && ok;
    ok      = expectEqualInt(packetData(packet)[translateToIndexOffset], 1, "ToIndex") && ok;
    ok      = expectBytes(packet, translateFromStringOffset, stringBytes("yagudo"), "FromString") && ok;
    ok      = expectBytes(packet, translateToStringOffset, stringBytes("yagudo drink"), "ToString") && ok;
    ok      = expectZeroRange(packet, translatePacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testRawEmbeddedNulCopy() -> bool
{
    auto packet = GP_SERV_COMMAND_TRANSLATE(0x5566, GP_CLI_COMMAND_TRANSLATE_INDEX::English, GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese, std::string("ab\0cd", 5), std::string("jp\0raw", 6));

    bool ok = true;
    ok      = expectEqualInt(packetData(packet)[translateFromIndexOffset], 1, "embedded FromIndex") && ok;
    ok      = expectEqualInt(packetData(packet)[translateToIndexOffset], 0, "embedded ToIndex") && ok;
    ok      = expectBytes(packet, translateFromStringOffset, stringBytes(std::string_view("ab\0cd", 5)), "embedded FromString") && ok;
    ok      = expectBytes(packet, translateToStringOffset, stringBytes(std::string_view("jp\0raw", 6)), "embedded ToString") && ok;
    return ok;
}

auto testLongStringsTruncateWithoutTerminator() -> bool
{
    auto from = std::string(70, 'f');
    auto to   = std::string(70, 't');
    from[63]  = 'F';
    from[64]  = 'x';
    to[63]    = 'T';
    to[64]    = 'y';

    auto packet = GP_SERV_COMMAND_TRANSLATE(0x7788, GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese, GP_CLI_COMMAND_TRANSLATE_INDEX::English, from, to);

    auto expectedFrom = std::string(64, 'f');
    auto expectedTo   = std::string(64, 't');
    expectedFrom[63]  = 'F';
    expectedTo[63]    = 'T';

    bool ok = true;
    ok      = expectBytes(packet, translateFromStringOffset, stringBytes(expectedFrom), "long FromString") && ok;
    ok      = expectBytes(packet, translateToStringOffset, stringBytes(expectedTo), "long ToString") && ok;
    ok      = expectEqualInt(packetData(packet)[translateFromStringOffset + 63], 'F', "long FromString[63]") && ok;
    ok      = expectEqualInt(packetData(packet)[translateToStringOffset + 63], 'T', "long ToString[63]") && ok;
    return ok;
}

auto testEmptyStringsZeroPayloadArrays() -> bool
{
    auto packet = GP_SERV_COMMAND_TRANSLATE(0, GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese, GP_CLI_COMMAND_TRANSLATE_INDEX::Japanese, "", "");

    bool ok = true;
    ok      = expectZeroRange(packet, translateFromStringOffset, translateFromStringOffset + 64, "empty FromString") && ok;
    ok      = expectZeroRange(packet, translateToStringOffset, translateToStringOffset + 64, "empty ToString") && ok;
    return ok;
}

} // namespace

auto runS2CTranslatePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    ok      = testRawEmbeddedNulCopy() && ok;
    ok      = testLongStringsTruncateWithoutTerminator() && ok;
    ok      = testEmptyStringsZeroPayloadArrays() && ok;
    return ok;
}
