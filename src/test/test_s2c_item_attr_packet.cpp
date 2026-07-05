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

#include "test_s2c_item_attr_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/enums/item_lockflg.h"
#include "map/item_container.h"
#include "map/items/item.h"
#include "map/packets/s2c/0x020_item_attr.h"

namespace
{

constexpr auto itemAttrItemNumOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_ATTR::PacketData, ItemNum);
constexpr auto itemAttrPriceOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_ATTR::PacketData, Price);
constexpr auto itemAttrItemNoOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_ATTR::PacketData, ItemNo);
constexpr auto itemAttrCategoryOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_ATTR::PacketData, Category);
constexpr auto itemAttrItemIndexOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_ATTR::PacketData, ItemIndex);
constexpr auto itemAttrLockFlgOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_ATTR::PacketData, LockFlg);
constexpr auto itemAttrAttrOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_ATTR::PacketData, Attr);
constexpr auto itemAttrPacketDataSize     = sizeof(GP_SERV_COMMAND_ITEM_ATTR::PacketData);
constexpr auto itemAttrUnroundedPacketSize = sizeof(GP_SERV_HEADER) + itemAttrPacketDataSize;
constexpr auto itemAttrRoundedPacketSize  = 44U;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ITEM_ATTR packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ITEM_ATTR packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ITEM_ATTR packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto makeItem() -> CItem
{
    auto item = CItem(0x2345);
    item.setStackSize(0xFFFFFFFF);
    item.setQuantity(0x01020304);
    item.setCharPrice(0x11223344);
    for (std::size_t i = 0; i < CItem::extra_size; ++i)
    {
        item.m_extra[i] = static_cast<uint8>(0x80 + i);
    }
    return item;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(itemAttrPacketDataSize, 37, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(itemAttrUnroundedPacketSize, 41, "unrounded packet size") && ok;
    ok      = expectEqualUInt(itemAttrItemNumOffset, 4, "ItemNum offset") && ok;
    ok      = expectEqualUInt(itemAttrPriceOffset, 8, "Price offset") && ok;
    ok      = expectEqualUInt(itemAttrItemNoOffset, 12, "ItemNo offset") && ok;
    ok      = expectEqualUInt(itemAttrCategoryOffset, 14, "Category offset") && ok;
    ok      = expectEqualUInt(itemAttrItemIndexOffset, 15, "ItemIndex offset") && ok;
    ok      = expectEqualUInt(itemAttrLockFlgOffset, 16, "LockFlg offset") && ok;
    ok      = expectEqualUInt(itemAttrAttrOffset, 17, "Attr offset") && ok;
    ok      = expectEqualUInt(LOC_WARDROBE3, 11, "LOC_WARDROBE3") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(ItemLockFlg::Unknown0), 0x19, "ItemLockFlg::Unknown0") && ok;
    return ok;
}

auto testNullConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_ITEM_ATTR(nullptr, LOC_STORAGE, 0x22);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x020, "ITEM_ATTR type") && ok;
    ok      = expectEqualUInt(packet.getSize(), itemAttrRoundedPacketSize, "ITEM_ATTR rounded size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 18>{ 0x20, 0x16, 0xEF, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x00, 0x00 }, "null constructor prefix") && ok;
    ok      = expectZeroTail(packet, 18, "null constructor tail") && ok;
    return ok;
}

auto testItemConstructor() -> bool
{
    auto item   = makeItem();
    auto packet = GP_SERV_COMMAND_ITEM_ATTR(&item, LOC_WARDROBE3, 0x22);

    const auto expectedPayload = std::array<uint8, 37>{
        0x04, 0x03, 0x02, 0x01,
        0x44, 0x33, 0x22, 0x11,
        0x45, 0x23,
        0x0B, 0x22, 0x19,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    };

    bool ok = true;
    ok      = expectBytes(packet, itemAttrItemNumOffset, expectedPayload, "item constructor payload") && ok;
    ok      = expectZeroTail(packet, itemAttrRoundedPacketSize, "item constructor tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemAttrPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testNullConstructor() && ok;
    ok      = testItemConstructor() && ok;
    return ok;
}
