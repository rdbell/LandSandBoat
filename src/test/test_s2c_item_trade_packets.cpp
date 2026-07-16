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

#include "test_s2c_item_trade_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/items/item.h"
#include "map/packets/s2c/0x023_item_trade_list.h"
#include "map/packets/s2c/0x025_item_trade_mylist.h"

namespace
{

constexpr auto tradeListItemNumOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData, ItemNum);
constexpr auto tradeListTradeCounterOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData, TradeCounter);
constexpr auto tradeListItemNoOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData, ItemNo);
constexpr auto tradeListItemFreeSpaceNumOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData, ItemFreeSpaceNum);
constexpr auto tradeListTradeIndexOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData, TradeIndex);
constexpr auto tradeListAttrOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData, Attr);
constexpr auto tradeListPacketDataSize         = sizeof(GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData);
constexpr auto tradeListUnroundedPacketSize    = sizeof(GP_SERV_HEADER) + tradeListPacketDataSize;
constexpr auto tradeListRoundedPacketSize      = 40U;

constexpr auto myListItemNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_MYLIST::PacketData, ItemNum);
constexpr auto myListItemNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_MYLIST::PacketData, ItemNo);
constexpr auto myListTradeIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_MYLIST::PacketData, TradeIndex);
constexpr auto myListItemIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_TRADE_MYLIST::PacketData, ItemIndex);
constexpr auto myListPacketDataSize   = sizeof(GP_SERV_COMMAND_ITEM_TRADE_MYLIST::PacketData);
constexpr auto myListFullPacketSize   = sizeof(GP_SERV_HEADER) + myListPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c item trade packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c item trade packet self-test failed: " << label << " got";
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
            std::cerr << "s2c item trade packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto makeTradeItem(uint32 reserve) -> CItem
{
    auto item = CItem(0x3456);
    item.setStackSize(0xFFFFFFFF);
    item.setQuantity(0x01020304);
    item.setReserve(reserve);
    item.setSlotID(0x22);
    for (std::size_t i = 0; i < CItem::extra_size; ++i)
    {
        item.m_extra[i] = static_cast<uint8>(0xA0 + i);
    }
    return item;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(tradeListPacketDataSize, 36, "sizeof(ITEM_TRADE_LIST::PacketData)") && ok;
    ok      = expectEqualUInt(tradeListUnroundedPacketSize, 40, "ITEM_TRADE_LIST unrounded packet size") && ok;
    ok      = expectEqualUInt(tradeListItemNumOffset, 4, "ITEM_TRADE_LIST ItemNum offset") && ok;
    ok      = expectEqualUInt(tradeListTradeCounterOffset, 8, "ITEM_TRADE_LIST TradeCounter offset") && ok;
    ok      = expectEqualUInt(tradeListItemNoOffset, 10, "ITEM_TRADE_LIST ItemNo offset") && ok;
    ok      = expectEqualUInt(tradeListItemFreeSpaceNumOffset, 12, "ITEM_TRADE_LIST ItemFreeSpaceNum offset") && ok;
    ok      = expectEqualUInt(tradeListTradeIndexOffset, 13, "ITEM_TRADE_LIST TradeIndex offset") && ok;
    ok      = expectEqualUInt(tradeListAttrOffset, 14, "ITEM_TRADE_LIST Attr offset") && ok;
    ok      = expectEqualUInt(myListPacketDataSize, 8, "sizeof(ITEM_TRADE_MYLIST::PacketData)") && ok;
    ok      = expectEqualUInt(myListFullPacketSize, 12, "ITEM_TRADE_MYLIST full packet size") && ok;
    ok      = expectEqualUInt(myListItemNumOffset, 4, "ITEM_TRADE_MYLIST ItemNum offset") && ok;
    ok      = expectEqualUInt(myListItemNoOffset, 8, "ITEM_TRADE_MYLIST ItemNo offset") && ok;
    ok      = expectEqualUInt(myListTradeIndexOffset, 10, "ITEM_TRADE_MYLIST TradeIndex offset") && ok;
    ok      = expectEqualUInt(myListItemIndexOffset, 11, "ITEM_TRADE_MYLIST ItemIndex offset") && ok;
    return ok;
}

auto testTradeListConstructor() -> bool
{
    auto item   = makeTradeItem(0x00010203);
    auto packet = GP_SERV_COMMAND_ITEM_TRADE_LIST(&item, 0x07);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 38>{
        0x23,
        0x14,
        0xEF,
        0xBE,
        0x03,
        0x02,
        0x01,
        0x00,
        0x00,
        0x00,
        0x56,
        0x34,
        0x00,
        0x07,
        0xA0,
        0xA1,
        0xA2,
        0xA3,
        0xA4,
        0xA5,
        0xA6,
        0xA7,
        0xA8,
        0xA9,
        0xAA,
        0xAB,
        0xAC,
        0xAD,
        0xAE,
        0xAF,
        0xB0,
        0xB1,
        0xB2,
        0xB3,
        0xB4,
        0xB5,
        0xB6,
        0xB7,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x023, "ITEM_TRADE_LIST type") && ok;
    ok      = expectEqualUInt(packet.getSize(), tradeListRoundedPacketSize, "ITEM_TRADE_LIST rounded size") && ok;
    ok      = expectBytes(packet, 0, expected, "ITEM_TRADE_LIST constructor bytes") && ok;
    ok      = expectZeroTail(packet, tradeListRoundedPacketSize, "ITEM_TRADE_LIST tail") && ok;
    return ok;
}

auto testMyListConstructor() -> bool
{
    auto item   = makeTradeItem(0x00010203);
    auto packet = GP_SERV_COMMAND_ITEM_TRADE_MYLIST(&item, 0x07);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x025, "ITEM_TRADE_MYLIST type") && ok;
    ok      = expectEqualUInt(packet.getSize(), myListFullPacketSize, "ITEM_TRADE_MYLIST size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 12>{ 0x25, 0x06, 0xEF, 0xBE, 0x03, 0x02, 0x01, 0x00, 0x56, 0x34, 0x07, 0x22 }, "ITEM_TRADE_MYLIST constructor bytes") && ok;
    ok      = expectZeroTail(packet, myListFullPacketSize, "ITEM_TRADE_MYLIST tail") && ok;
    return ok;
}

auto testMyListZeroReserveConstructor() -> bool
{
    auto item   = makeTradeItem(0);
    auto packet = GP_SERV_COMMAND_ITEM_TRADE_MYLIST(&item, 0x07);

    bool ok = true;
    ok      = expectBytes(packet, myListItemNumOffset, std::array<uint8, 8>{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00 }, "ITEM_TRADE_MYLIST zero reserve payload") && ok;
    ok      = expectZeroTail(packet, myListFullPacketSize, "ITEM_TRADE_MYLIST zero reserve tail") && ok;
    return ok;
}

auto testMyListRuntimePlanReserveBranches() -> bool
{
    const auto present = itemtrademylisthelpers::PlanFor(0x00010203, 0x3456, 0x07, 0x22);
    const auto empty   = itemtrademylisthelpers::PlanFor(0, 0x3456, 0x07, 0x22);

    bool ok = true;
    ok      = expectEqualUInt(present.ItemNum, 0x00010203, "ITEM_TRADE_MYLIST present reserve") && ok;
    ok      = expectEqualUInt(present.ItemNo, 0x3456, "ITEM_TRADE_MYLIST present item ID") && ok;
    ok      = expectEqualUInt(present.TradeIndex, 0x07, "ITEM_TRADE_MYLIST present trade index") && ok;
    ok      = expectEqualUInt(present.ItemIndex, 0x22, "ITEM_TRADE_MYLIST present item slot") && ok;
    ok      = expectEqualUInt(empty.ItemNum, 0, "ITEM_TRADE_MYLIST empty reserve") && ok;
    ok      = expectEqualUInt(empty.ItemNo, 0, "ITEM_TRADE_MYLIST empty item ID") && ok;
    ok      = expectEqualUInt(empty.TradeIndex, 0x07, "ITEM_TRADE_MYLIST empty trade index retained") && ok;
    ok      = expectEqualUInt(empty.ItemIndex, 0, "ITEM_TRADE_MYLIST empty item slot") && ok;
    return ok;
}

} // namespace

auto runS2CItemTradePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testTradeListConstructor() && ok;
    ok      = testMyListConstructor() && ok;
    ok      = testMyListZeroReserveConstructor() && ok;
    ok      = testMyListRuntimePlanReserveBranches() && ok;
    return ok;
}
