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

#include "test_s2c_black_list_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "map/packets/s2c/0x041_black_list.h"

namespace
{

constexpr auto blackListListOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BLACK_LIST::PacketData, List);
constexpr auto blackListStatOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BLACK_LIST::PacketData, Stat);
constexpr auto blackListNumOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BLACK_LIST::PacketData, Num);
constexpr auto blackListPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BLACK_LIST::PacketData, padding00);
constexpr auto blackListPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_BLACK_LIST::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BLACK_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BLACK_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BLACK_LIST packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
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
        std::cerr << "s2c BLACK_LIST packet self-test failed: " << label << " got";
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
            std::cerr << "s2c BLACK_LIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto listEntryBytes(std::uint32_t id, std::string_view name) -> std::array<uint8, sizeof(SAVE_BLACK)>
{
    auto bytes = std::array<uint8, sizeof(SAVE_BLACK)>{};
    bytes[0]   = static_cast<uint8>(id);
    bytes[1]   = static_cast<uint8>(id >> 8);
    bytes[2]   = static_cast<uint8>(id >> 16);
    bytes[3]   = static_cast<uint8>(id >> 24);
    std::memcpy(bytes.data() + offsetof(SAVE_BLACK, Name), name.data(), std::min<std::size_t>(name.size(), sizeof(SAVE_BLACK::Name)));
    return bytes;
}

auto makeEntries(std::size_t count) -> std::vector<std::pair<uint32, std::string>>
{
    auto entries = std::vector<std::pair<uint32, std::string>>{};
    entries.reserve(count);
    for (std::size_t i = 0; i < count; ++i)
    {
        entries.emplace_back(0x1000 + static_cast<uint32>(i), "name" + std::to_string(i));
    }
    return entries;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(SAVE_BLACK), 20, "sizeof(SAVE_BLACK)") && ok;
    ok      = expectEqualUInt(offsetof(SAVE_BLACK, ID), 0, "SAVE_BLACK ID offset") && ok;
    ok      = expectEqualUInt(offsetof(SAVE_BLACK, Name), 4, "SAVE_BLACK Name offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_BLACK_LIST::PacketData), 244, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(blackListPacketSize, 248, "packet size") && ok;
    ok      = expectEqualUInt(blackListListOffset, 4, "List offset") && ok;
    ok      = expectEqualUInt(blackListStatOffset, 244, "Stat offset") && ok;
    ok      = expectEqualUInt(blackListNumOffset, 245, "Num offset") && ok;
    ok      = expectEqualUInt(blackListPadding00Offset, 246, "padding00 offset") && ok;
    return ok;
}

auto testEmptyConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_BLACK_LIST({}, GP_SERV_COMMAND_BLACK_LIST::ResetClientBlacklist::No, GP_SERV_COMMAND_BLACK_LIST::LastBlacklistPacket::No);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x041, "empty type") && ok;
    ok      = expectEqualUInt(packet.getSize(), blackListPacketSize, "empty size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x41, 0x7C, 0xEF, 0xBE }, "empty header") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListStatOffset]), 0, "empty Stat") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListNumOffset]), 0, "empty Num") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), blackListPacketSize, "empty payload") && ok;
    ok      = expectZeroRange(packet, blackListPacketSize, PACKET_SIZE, "empty tail") && ok;
    return ok;
}

auto testFlaggedEntriesConstructor() -> bool
{
    auto entries = std::vector<std::pair<uint32, std::string>>{
        { 0x11223344, "Alice" },
        { 0x55667788, std::string("Bob\0Raw", 7) },
    };

    auto packet = GP_SERV_COMMAND_BLACK_LIST(entries, GP_SERV_COMMAND_BLACK_LIST::ResetClientBlacklist::Yes, GP_SERV_COMMAND_BLACK_LIST::LastBlacklistPacket::Yes);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x41, 0x7C, 0xEF, 0xBE }, "flagged header") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListStatOffset]), 0x03, "flagged Stat") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListNumOffset]), 2, "flagged Num") && ok;
    ok      = expectBytes(packet, blackListListOffset, listEntryBytes(0x11223344, "Alice"), "first entry") && ok;
    ok      = expectBytes(packet, blackListListOffset + sizeof(SAVE_BLACK), listEntryBytes(0x55667788, std::string_view("Bob\0Raw", 7)), "second entry copies embedded nul") && ok;
    ok      = expectZeroRange(packet, blackListListOffset + 2 * sizeof(SAVE_BLACK), blackListStatOffset, "unused entries") && ok;
    ok      = expectZeroRange(packet, blackListPadding00Offset, blackListPacketSize, "flagged padding") && ok;
    ok      = expectZeroRange(packet, blackListPacketSize, PACKET_SIZE, "flagged tail") && ok;
    return ok;
}

auto testThirteenEntriesReportsFullCountButOnlyEncodesFirstTwelve() -> bool
{
    auto entries = makeEntries(13);
    auto packet  = GP_SERV_COMMAND_BLACK_LIST(entries, GP_SERV_COMMAND_BLACK_LIST::ResetClientBlacklist::No, GP_SERV_COMMAND_BLACK_LIST::LastBlacklistPacket::Yes);

    bool ok = true;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListStatOffset]), 0x02, "thirteen Stat") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListNumOffset]), 13, "thirteen Num") && ok;
    ok      = expectBytes(packet, blackListListOffset, listEntryBytes(0x1000, "name0"), "thirteen first entry") && ok;
    ok      = expectBytes(packet, blackListListOffset + 11 * sizeof(SAVE_BLACK), listEntryBytes(0x100B, "name11"), "thirteen twelfth entry") && ok;

    const auto encoded = std::string_view(reinterpret_cast<const char*>(packetData(packet) + blackListListOffset + 11 * sizeof(SAVE_BLACK) + offsetof(SAVE_BLACK, Name)), sizeof(SAVE_BLACK::Name));
    ok                 = expectEqualString(encoded.find("name12") == std::string_view::npos ? "absent" : "present", "absent", "thirteenth entry absent") && ok;
    return ok;
}

auto testLongNameTruncatesWithoutTerminator() -> bool
{
    auto packet = GP_SERV_COMMAND_BLACK_LIST({ { 0xAABBCCDD, "abcdefghijklmnopZ" } }, GP_SERV_COMMAND_BLACK_LIST::ResetClientBlacklist::Yes, GP_SERV_COMMAND_BLACK_LIST::LastBlacklistPacket::No);

    bool ok = true;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListStatOffset]), 0x01, "long-name Stat") && ok;
    ok      = expectEqualInt(static_cast<int8_t>(packetData(packet)[blackListNumOffset]), 1, "long-name Num") && ok;
    ok      = expectBytes(packet, blackListListOffset, listEntryBytes(0xAABBCCDD, "abcdefghijklmnop"), "long-name entry") && ok;
    return ok;
}

} // namespace

auto runS2CBlackListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEmptyConstructor() && ok;
    ok      = testFlaggedEntriesConstructor() && ok;
    ok      = testThirteenEntriesReportsFullCountButOnlyEncodesFirstTwelve() && ok;
    ok      = testLongNameTruncatesWithoutTerminator() && ok;
    return ok;
}
