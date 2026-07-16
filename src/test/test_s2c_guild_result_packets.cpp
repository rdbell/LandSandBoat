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

#include "test_s2c_guild_result_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x082_guild_buy.h"
#include "map/packets/s2c/0x084_guild_sell.h"
#include "map/packets/s2c/0x086_guild_open.h"

namespace
{

constexpr auto guildBuyItemNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_BUY::PacketData, ItemNo);
constexpr auto guildBuyCountOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_BUY::PacketData, Count);
constexpr auto guildBuyTradeOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_BUY::PacketData, Trade);
constexpr auto guildBuyPacketDataSize = sizeof(GP_SERV_COMMAND_GUILD_BUY::PacketData);
constexpr auto guildBuyPacketSize     = sizeof(GP_SERV_HEADER) + guildBuyPacketDataSize;

constexpr auto guildSellItemNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_SELL::PacketData, ItemNo);
constexpr auto guildSellCountOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_SELL::PacketData, Count);
constexpr auto guildSellTradeOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_SELL::PacketData, Trade);
constexpr auto guildSellPacketDataSize = sizeof(GP_SERV_COMMAND_GUILD_SELL::PacketData);
constexpr auto guildSellPacketSize     = sizeof(GP_SERV_HEADER) + guildSellPacketDataSize;

constexpr auto guildOpenStatOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_OPEN::PacketData, Stat);
constexpr auto guildOpenPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_OPEN::PacketData, padding00);
constexpr auto guildOpenTimeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GUILD_OPEN::PacketData, Time);
constexpr auto guildOpenPacketDataSize  = sizeof(GP_SERV_COMMAND_GUILD_OPEN::PacketData);
constexpr auto guildOpenPacketSize      = sizeof(GP_SERV_HEADER) + guildOpenPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c guild result packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c guild result packet self-test failed: " << label << " got";
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

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c guild result packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(guildBuyPacketDataSize, 4, "sizeof(GUILD_BUY::PacketData)") && ok;
    ok      = expectEqualUInt(guildBuyPacketSize, 8, "GUILD_BUY packet size") && ok;
    ok      = expectEqualUInt(guildBuyItemNoOffset, 4, "GUILD_BUY ItemNo offset") && ok;
    ok      = expectEqualUInt(guildBuyCountOffset, 6, "GUILD_BUY Count offset") && ok;
    ok      = expectEqualUInt(guildBuyTradeOffset, 7, "GUILD_BUY Trade offset") && ok;

    ok = expectEqualUInt(guildSellPacketDataSize, 4, "sizeof(GUILD_SELL::PacketData)") && ok;
    ok = expectEqualUInt(guildSellPacketSize, 8, "GUILD_SELL packet size") && ok;
    ok = expectEqualUInt(guildSellItemNoOffset, 4, "GUILD_SELL ItemNo offset") && ok;
    ok = expectEqualUInt(guildSellCountOffset, 6, "GUILD_SELL Count offset") && ok;
    ok = expectEqualUInt(guildSellTradeOffset, 7, "GUILD_SELL Trade offset") && ok;

    ok = expectEqualUInt(guildOpenPacketDataSize, 8, "sizeof(GUILD_OPEN::PacketData)") && ok;
    ok = expectEqualUInt(guildOpenPacketSize, 12, "GUILD_OPEN packet size") && ok;
    ok = expectEqualUInt(guildOpenStatOffset, 4, "GUILD_OPEN Stat offset") && ok;
    ok = expectEqualUInt(guildOpenPadding00Offset, 5, "GUILD_OPEN padding00 offset") && ok;
    ok = expectEqualUInt(guildOpenTimeOffset, 8, "GUILD_OPEN Time offset") && ok;
    ok = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_GUILD_OPEN_STAT::Open), 0, "GUILD_OPEN_STAT::Open") && ok;
    ok = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_GUILD_OPEN_STAT::Close), 1, "GUILD_OPEN_STAT::Close") && ok;
    ok = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_GUILD_OPEN_STAT::Holiday), 2, "GUILD_OPEN_STAT::Holiday") && ok;
    return ok;
}

auto testBuyAndSellConstructors() -> bool
{
    auto character = CCharEntity{};
    auto buy       = GP_SERV_COMMAND_GUILD_BUY(&character, 0x12, 0x2345, 0x7F);
    buy.setSequence(0xBEEF);
    auto sell = GP_SERV_COMMAND_GUILD_SELL(&character, 0x34, 0x4567, 0x7E);
    sell.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(buy.getType(), 0x082, "GUILD_BUY type") && ok;
    ok      = expectEqualUInt(buy.getSize(), guildBuyPacketSize, "GUILD_BUY size") && ok;
    ok      = expectBytes(buy, 0, std::array<uint8, 8>{ 0x82, 0x04, 0xEF, 0xBE, 0x45, 0x23, 0x12, 0x7F }, "GUILD_BUY bytes") && ok;
    ok      = expectZeroTail(buy, guildBuyPacketSize, "GUILD_BUY tail") && ok;

    ok = expectEqualUInt(sell.getType(), 0x084, "GUILD_SELL type") && ok;
    ok = expectEqualUInt(sell.getSize(), guildSellPacketSize, "GUILD_SELL size") && ok;
    ok = expectBytes(sell, 0, std::array<uint8, 8>{ 0x84, 0x04, 0xEF, 0xBE, 0x67, 0x45, 0x34, 0x7E }, "GUILD_SELL bytes") && ok;
    ok = expectZeroTail(sell, guildSellPacketSize, "GUILD_SELL tail") && ok;
    return ok;
}

auto testSellConstructorPreservesTradeByte() -> bool
{
    auto character = CCharEntity{};
    auto sell      = GP_SERV_COMMAND_GUILD_SELL(&character, 0, 0, static_cast<uint8>(-4));

    return expectBytes(sell, guildSellTradeOffset, std::array<uint8, 1>{ 0xFC }, "GUILD_SELL signed trade byte");
}

auto testGuildOpenConstructors() -> bool
{
    auto open = GP_SERV_COMMAND_GUILD_OPEN(GP_SERV_COMMAND_GUILD_OPEN_STAT::Open, 6, 18, 0);
    open.setSequence(0xBEEF);
    auto holiday = GP_SERV_COMMAND_GUILD_OPEN(GP_SERV_COMMAND_GUILD_OPEN_STAT::Holiday, 0, 0, 9);
    holiday.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(open.getType(), 0x086, "GUILD_OPEN type") && ok;
    ok      = expectEqualUInt(open.getSize(), guildOpenPacketSize, "GUILD_OPEN size") && ok;
    ok      = expectBytes(open, 0, std::array<uint8, 12>{ 0x86, 0x06, 0xEF, 0xBE, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x03, 0x00 }, "GUILD_OPEN open bytes") && ok;
    ok      = expectZeroTail(open, guildOpenPacketSize, "GUILD_OPEN open tail") && ok;

    ok = expectBytes(holiday, 0, std::array<uint8, 12>{ 0x86, 0x06, 0xEF, 0xBE, 0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00 }, "GUILD_OPEN holiday bytes") && ok;
    ok = expectZeroTail(holiday, guildOpenPacketSize, "GUILD_OPEN holiday tail") && ok;
    return ok;
}

} // namespace

auto runS2CGuildResultPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testBuyAndSellConstructors() && ok;
    ok      = testSellConstructorPreservesTradeByte() && ok;
    ok      = testGuildOpenConstructors() && ok;
    return ok;
}
