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

#include "test_s2c_guild_list_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x083_guild_buylist.h"
#include "map/packets/s2c/0x085_guild_selllist.h"

namespace
{

constexpr auto guildListItemLen          = 30U;
constexpr auto guildListItemSize         = sizeof(GP_GUILD_ITEM);
constexpr auto guildBuyListListOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_BUYLIST::PacketData, List);
constexpr auto guildBuyListCountOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_BUYLIST::PacketData, Count);
constexpr auto guildBuyListStatOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_BUYLIST::PacketData, Stat);
constexpr auto guildBuyListPacketSize    = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_GUILD_BUYLIST::PacketData);
constexpr auto guildSellListListOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_SELLLIST::PacketData, List);
constexpr auto guildSellListCountOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_SELLLIST::PacketData, Count);
constexpr auto guildSellListStatOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_SELLLIST::PacketData, Stat);
constexpr auto guildSellListPacketSize   = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_GUILD_SELLLIST::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c guild list packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c guild list packet self-test failed: " << label << " got";
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

template <typename T, std::size_t Size>
auto expectStructBytes(const T& value, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = reinterpret_cast<const uint8*>(&value);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c guild list packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto valueByte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(valueByte);
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
            std::cerr << "s2c guild list packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto sampleItems() -> std::vector<GP_GUILD_ITEM>
{
    return {
        GP_GUILD_ITEM{ .ItemNo = 0x1234, .Count = 0x05, .Max = 0x09, .Price = 0x01020304 },
        GP_GUILD_ITEM{ .ItemNo = 0x5678, .Count = 0x06, .Max = 0x0A, .Price = -7 },
    };
}

auto numberedItems(const std::size_t count) -> std::vector<GP_GUILD_ITEM>
{
    auto items = std::vector<GP_GUILD_ITEM>{};
    items.reserve(count);
    for (std::size_t i = 0; i < count; ++i)
    {
        items.push_back(GP_GUILD_ITEM{
            .ItemNo = static_cast<uint16>(0x1000 + i),
            .Count  = static_cast<uint8>(i + 1),
            .Max    = static_cast<uint8>(i + 2),
            .Price  = static_cast<int32>(1000 + i),
        });
    }
    return items;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(guildListItemSize, 8, "sizeof(GP_GUILD_ITEM)") && ok;
    ok      = expectEqualUInt(guildListItemLen, 30, "std::size(List)") && ok;
    ok      = expectEqualUInt(guildBuyListPacketSize, 248, "GUILD_BUYLIST packet size") && ok;
    ok      = expectEqualUInt(guildBuyListListOffset, 4, "GUILD_BUYLIST List offset") && ok;
    ok      = expectEqualUInt(guildBuyListCountOffset, 244, "GUILD_BUYLIST Count offset") && ok;
    ok      = expectEqualUInt(guildBuyListStatOffset, 245, "GUILD_BUYLIST Stat offset") && ok;
    ok      = expectEqualUInt(guildSellListPacketSize, 248, "GUILD_SELLLIST packet size") && ok;
    ok      = expectEqualUInt(guildSellListListOffset, 4, "GUILD_SELLLIST List offset") && ok;
    ok      = expectEqualUInt(guildSellListCountOffset, 244, "GUILD_SELLLIST Count offset") && ok;
    ok      = expectEqualUInt(guildSellListStatOffset, 245, "GUILD_SELLLIST Stat offset") && ok;

    auto item = GP_GUILD_ITEM{ .ItemNo = 0x1234, .Count = 0x05, .Max = 0x09, .Price = 0x01020304 };
    ok        = expectStructBytes(item, std::array<uint8, 8>{ 0x34, 0x12, 0x05, 0x09, 0x04, 0x03, 0x02, 0x01 }, "GP_GUILD_ITEM bytes") && ok;
    return ok;
}

auto testBuyListConstructor() -> bool
{
    auto character = CCharEntity{};
    auto packet    = GP_SERV_COMMAND_GUILD_BUYLIST(&character, sampleItems());
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x083, "GUILD_BUYLIST type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 248, "GUILD_BUYLIST rounded size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 22>{
                                      0x83, 0x7C, 0xEF, 0xBE,
                                      0x34, 0x12, 0x05, 0x09,
                                      0x04, 0x03, 0x02, 0x01,
                                      0x78, 0x56, 0x06, 0x0A,
                                      0xF9, 0xFF, 0xFF, 0xFF,
                                      0x00, 0x00,
                                  },
                      "GUILD_BUYLIST prefix") &&
              ok;
    ok = expectBytes(packet, guildBuyListCountOffset, std::array<uint8, 2>{ 0x02, 0x80 }, "GUILD_BUYLIST count/stat") && ok;
    ok = expectZeroTail(packet, guildBuyListPacketSize, "GUILD_BUYLIST tail") && ok;
    return ok;
}

auto testSellListConstructor() -> bool
{
    auto character = CCharEntity{};
    auto packet    = GP_SERV_COMMAND_GUILD_SELLLIST(&character, sampleItems());
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x085, "GUILD_SELLLIST type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 248, "GUILD_SELLLIST rounded size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 22>{
                                      0x85, 0x7C, 0xEF, 0xBE,
                                      0x34, 0x12, 0x05, 0x09,
                                      0x04, 0x03, 0x02, 0x01,
                                      0x78, 0x56, 0x06, 0x0A,
                                      0xF9, 0xFF, 0xFF, 0xFF,
                                      0x00, 0x00,
                                  },
                      "GUILD_SELLLIST prefix") &&
              ok;
    ok = expectBytes(packet, guildSellListCountOffset, std::array<uint8, 2>{ 0x02, 0x80 }, "GUILD_SELLLIST count/stat") && ok;
    ok = expectZeroTail(packet, guildSellListPacketSize, "GUILD_SELLLIST tail") && ok;
    return ok;
}

auto testSellListConstructorPaginates() -> bool
{
    auto character = CCharEntity{};
    auto packet    = GP_SERV_COMMAND_GUILD_SELLLIST(&character, numberedItems(31));

    const auto& pushedPackets = character.getPacketList();
    bool        ok            = true;
    ok                        = expectEqualUInt(pushedPackets.size(), 1, "GUILD_SELLLIST full page count") && ok;
    if (!pushedPackets.empty())
    {
        auto& first = *pushedPackets.front();
        ok          = expectEqualUInt(first.getType(), 0x085, "GUILD_SELLLIST pushed page type") && ok;
        ok          = expectBytes(first, guildSellListCountOffset, std::array<uint8, 2>{ 30, 0x40 }, "GUILD_SELLLIST pushed page count/stat") && ok;
        ok          = expectBytes(first, guildSellListListOffset, std::array<uint8, 8>{ 0x00, 0x10, 0x01, 0x02, 0xE8, 0x03, 0x00, 0x00 }, "GUILD_SELLLIST pushed first item") && ok;
        ok          = expectZeroTail(first, guildSellListPacketSize, "GUILD_SELLLIST pushed page tail") && ok;
    }

    ok = expectEqualUInt(packet.getType(), 0x085, "GUILD_SELLLIST final page type") && ok;
    ok = expectBytes(packet, guildSellListCountOffset, std::array<uint8, 2>{ 0x01, 0x81 }, "GUILD_SELLLIST final page count/stat") && ok;
    ok = expectBytes(packet, guildSellListListOffset, std::array<uint8, 8>{ 0x1E, 0x10, 0x1F, 0x20, 0x06, 0x04, 0x00, 0x00 }, "GUILD_SELLLIST final item") && ok;
    ok = expectZeroTail(packet, guildSellListPacketSize, "GUILD_SELLLIST final page tail") && ok;
    return ok;
}

} // namespace

auto runS2CGuildListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testBuyListConstructor() && ok;
    ok      = testSellListConstructor() && ok;
    ok      = testSellListConstructorPaginates() && ok;
    return ok;
}
