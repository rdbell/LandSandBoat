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

#include "test_s2c_black_edit_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/s2c/0x042_black_edit.h"

namespace
{

constexpr auto blackEditDataOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BLACK_EDIT::PacketData, Data);
constexpr auto blackEditDataIDOffset    = blackEditDataOffset + offsetof(SAVE_BLACK, ID);
constexpr auto blackEditDataNameOffset  = blackEditDataOffset + offsetof(SAVE_BLACK, Name);
constexpr auto blackEditModeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BLACK_EDIT::PacketData, Mode);
constexpr auto blackEditPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BLACK_EDIT::PacketData, padding00);
constexpr auto blackEditPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_BLACK_EDIT::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BLACK_EDIT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BLACK_EDIT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c BLACK_EDIT packet self-test failed: " << label << " got";
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
            std::cerr << "s2c BLACK_EDIT packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto nameBytes(std::string_view name) -> std::array<uint8, sizeof(SAVE_BLACK::Name)>
{
    auto bytes = std::array<uint8, sizeof(SAVE_BLACK::Name)>{};
    std::memcpy(bytes.data(), name.data(), std::min<std::size_t>(name.size(), bytes.size()));
    return bytes;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(SAVE_BLACK), 20, "sizeof(SAVE_BLACK)") && ok;
    ok      = expectEqualUInt(offsetof(SAVE_BLACK, ID), 0, "SAVE_BLACK ID offset") && ok;
    ok      = expectEqualUInt(offsetof(SAVE_BLACK, Name), 4, "SAVE_BLACK Name offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_BLACK_EDIT::PacketData), 24, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(blackEditPacketSize, 28, "packet size") && ok;
    ok      = expectEqualUInt(blackEditDataOffset, 4, "Data offset") && ok;
    ok      = expectEqualUInt(blackEditDataIDOffset, 4, "Data.ID offset") && ok;
    ok      = expectEqualUInt(blackEditDataNameOffset, 8, "Data.Name offset") && ok;
    ok      = expectEqualUInt(blackEditModeOffset, 24, "Mode offset") && ok;
    ok      = expectEqualUInt(blackEditPadding00Offset, 25, "padding00 offset") && ok;
    return ok;
}

auto testAddConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_BLACK_EDIT(0x11223344, "Alice", GP_SERV_COMMAND_BLACK_EDIT_MODE::Add);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x042, "add type") && ok;
    ok      = expectEqualUInt(packet.getSize(), blackEditPacketSize, "add size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x42, 0x0E, 0xEF, 0xBE }, "add header") && ok;
    ok      = expectBytes(packet, blackEditDataOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "add ID") && ok;
    ok      = expectBytes(packet, blackEditDataNameOffset, nameBytes("Alice"), "add name") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackEditModeOffset]), 0, "add Mode") && ok;
    ok      = expectZeroRange(packet, blackEditPadding00Offset, blackEditPacketSize, "add padding") && ok;
    ok      = expectZeroRange(packet, blackEditPacketSize, PACKET_SIZE, "add tail") && ok;
    return ok;
}

auto testDeleteConstructorCopiesEmbeddedNul() -> bool
{
    auto packet = GP_SERV_COMMAND_BLACK_EDIT(0x55667788, std::string("Bob\0Raw", 7), GP_SERV_COMMAND_BLACK_EDIT_MODE::Delete);

    bool ok = true;
    ok      = expectBytes(packet, blackEditDataOffset, std::array<uint8, 4>{ 0x88, 0x77, 0x66, 0x55 }, "delete ID") && ok;
    ok      = expectBytes(packet, blackEditDataNameOffset, nameBytes(std::string_view("Bob\0Raw", 7)), "delete embedded-nul name") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackEditModeOffset]), 1, "delete Mode") && ok;
    ok      = expectZeroRange(packet, blackEditPadding00Offset, blackEditPacketSize, "delete padding") && ok;
    return ok;
}

auto testErrorConstructorZerosDataButWritesMode() -> bool
{
    auto packet = GP_SERV_COMMAND_BLACK_EDIT(0xAABBCCDD, "Ignored", GP_SERV_COMMAND_BLACK_EDIT_MODE::Error);

    bool ok = true;
    ok      = expectZeroRange(packet, blackEditDataOffset, blackEditModeOffset, "error data") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackEditModeOffset]), 2, "error Mode") && ok;
    ok      = expectZeroRange(packet, blackEditPadding00Offset, blackEditPacketSize, "error padding") && ok;
    return ok;
}

auto testLongNameTruncatesWithoutTerminator() -> bool
{
    auto packet = GP_SERV_COMMAND_BLACK_EDIT(0x01020304, "abcdefghijklmnopZ", GP_SERV_COMMAND_BLACK_EDIT_MODE::Add);

    bool ok = true;
    ok      = expectBytes(packet, blackEditDataNameOffset, nameBytes("abcdefghijklmnop"), "long name") && ok;
    ok      = expectEqualInt(packetData(packet)[blackEditDataNameOffset + sizeof(SAVE_BLACK::Name) - 1], 'p', "long Name[15]") && ok;
    return ok;
}

} // namespace

auto runS2CBlackEditPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testAddConstructor() && ok;
    ok      = testDeleteConstructorCopiesEmbeddedNul() && ok;
    ok      = testErrorConstructorZerosDataButWritesMode() && ok;
    ok      = testLongNameTruncatesWithoutTerminator() && ok;
    return ok;
}
