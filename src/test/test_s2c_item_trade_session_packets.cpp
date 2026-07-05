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

#include "test_s2c_item_trade_session_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x021_item_trade_req.h"
#include "map/packets/s2c/0x022_item_trade_res.h"

namespace
{

constexpr auto tradeReqUniqueNoOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_REQ::PacketData, UniqueNo);
constexpr auto tradeReqActIndexOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_REQ::PacketData, ActIndex);
constexpr auto tradeReqPadding00Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_REQ::PacketData, padding00);
constexpr auto tradeReqPacketDataSize      = sizeof(GP_SERV_COMMAND_ITEM_TRADE_REQ::PacketData);
constexpr auto tradeReqFullPacketSize      = sizeof(GP_SERV_HEADER) + tradeReqPacketDataSize;

constexpr auto tradeResUniqueNoOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_RES::PacketData, UniqueNo);
constexpr auto tradeResKindOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_RES::PacketData, Kind);
constexpr auto tradeResActIndexOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_RES::PacketData, ActIndex);
constexpr auto tradeResPacketDataSize      = sizeof(GP_SERV_COMMAND_ITEM_TRADE_RES::PacketData);
constexpr auto tradeResUnroundedPacketSize = sizeof(GP_SERV_HEADER) + tradeResPacketDataSize;
constexpr auto tradeResRoundedPacketSize   = 16U;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c item trade session packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c item trade session packet self-test failed: " << label << " got";
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
            std::cerr << "s2c item trade session packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto makeChar(std::uint32_t id, std::uint16_t targid) -> CCharEntity
{
    auto character = CCharEntity();
    character.id   = id;
    character.targid = targid;
    return character;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(tradeReqPacketDataSize, 8, "sizeof(ITEM_TRADE_REQ::PacketData)") && ok;
    ok      = expectEqualUInt(tradeReqFullPacketSize, 12, "ITEM_TRADE_REQ full packet size") && ok;
    ok      = expectEqualUInt(tradeReqUniqueNoOffset, 4, "ITEM_TRADE_REQ UniqueNo offset") && ok;
    ok      = expectEqualUInt(tradeReqActIndexOffset, 8, "ITEM_TRADE_REQ ActIndex offset") && ok;
    ok      = expectEqualUInt(tradeReqPadding00Offset, 10, "ITEM_TRADE_REQ padding00 offset") && ok;
    ok      = expectEqualUInt(tradeResPacketDataSize, 10, "sizeof(ITEM_TRADE_RES::PacketData)") && ok;
    ok      = expectEqualUInt(tradeResUnroundedPacketSize, 14, "ITEM_TRADE_RES unrounded packet size") && ok;
    ok      = expectEqualUInt(tradeResUniqueNoOffset, 4, "ITEM_TRADE_RES UniqueNo offset") && ok;
    ok      = expectEqualUInt(tradeResKindOffset, 8, "ITEM_TRADE_RES Kind offset") && ok;
    ok      = expectEqualUInt(tradeResActIndexOffset, 12, "ITEM_TRADE_RES ActIndex offset") && ok;
    ok      = expectEqualUInt(static_cast<uint32>(GP_ITEM_TRADE_RES_KIND::ErrYouTrade), 7, "GP_ITEM_TRADE_RES_KIND::ErrYouTrade") && ok;
    return ok;
}

auto testTradeReqConstructor() -> bool
{
    auto character = makeChar(0x11223344, 0x5566);
    auto packet    = GP_SERV_COMMAND_ITEM_TRADE_REQ(&character);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x021, "ITEM_TRADE_REQ type") && ok;
    ok      = expectEqualUInt(packet.getSize(), tradeReqFullPacketSize, "ITEM_TRADE_REQ size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 12>{ 0x21, 0x06, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11, 0x66, 0x55, 0x00, 0x00 }, "ITEM_TRADE_REQ constructor bytes") && ok;
    ok      = expectZeroTail(packet, tradeReqFullPacketSize, "ITEM_TRADE_REQ tail") && ok;
    return ok;
}

auto testTradeResConstructor() -> bool
{
    auto character = makeChar(0x11223344, 0x5566);
    auto packet    = GP_SERV_COMMAND_ITEM_TRADE_RES(&character, GP_ITEM_TRADE_RES_KIND::ErrYouTrade);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x022, "ITEM_TRADE_RES type") && ok;
    ok      = expectEqualUInt(packet.getSize(), tradeResRoundedPacketSize, "ITEM_TRADE_RES rounded size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 14>{ 0x22, 0x08, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11, 0x07, 0x00, 0x00, 0x00, 0x66, 0x55 }, "ITEM_TRADE_RES constructor bytes") && ok;
    ok      = expectZeroTail(packet, tradeResRoundedPacketSize, "ITEM_TRADE_RES tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemTradeSessionPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testTradeReqConstructor() && ok;
    ok      = testTradeResConstructor() && ok;
    return ok;
}
