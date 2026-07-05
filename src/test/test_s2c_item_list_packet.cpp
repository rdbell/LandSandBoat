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

#include "test_s2c_item_list_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/enums/item_lockflg.h"
#include "map/item_container.h"
#include "map/items/item.h"
#include "map/packets/s2c/0x01f_item_list.h"

namespace
{

constexpr auto itemListItemNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_LIST::PacketData, ItemNum);
constexpr auto itemListItemNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_LIST::PacketData, ItemNo);
constexpr auto itemListCategoryOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_LIST::PacketData, Category);
constexpr auto itemListItemIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_LIST::PacketData, ItemIndex);
constexpr auto itemListLockFlgOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_LIST::PacketData, LockFlg);
constexpr auto itemListPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_LIST::PacketData, padding00);
constexpr auto itemListPacketDataSize   = sizeof(GP_SERV_COMMAND_ITEM_LIST::PacketData);
constexpr auto itemListFullPacketSize   = sizeof(GP_SERV_HEADER) + itemListPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ITEM_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ITEM_LIST packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ITEM_LIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto makeItem() -> CItem
{
    auto item = CItem(0x1234);
    item.setQuantity(0x01020304);
    item.setLocationID(LOC_WARDROBE3);
    item.setSlotID(0x22);
    return item;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(itemListPacketDataSize, 12, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(itemListFullPacketSize, 16, "full packet size") && ok;
    ok      = expectEqualUInt(itemListItemNumOffset, 4, "ItemNum offset") && ok;
    ok      = expectEqualUInt(itemListItemNoOffset, 8, "ItemNo offset") && ok;
    ok      = expectEqualUInt(itemListCategoryOffset, 10, "Category offset") && ok;
    ok      = expectEqualUInt(itemListItemIndexOffset, 11, "ItemIndex offset") && ok;
    ok      = expectEqualUInt(itemListLockFlgOffset, 12, "LockFlg offset") && ok;
    ok      = expectEqualUInt(itemListPadding00Offset, 13, "padding00 offset") && ok;
    ok      = expectEqualUInt(LOC_WARDROBE3, 11, "LOC_WARDROBE3") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(ItemLockFlg::NoSelect), 0x0F, "ItemLockFlg::NoSelect") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto item   = makeItem();
    auto packet = GP_SERV_COMMAND_ITEM_LIST(&item, ItemLockFlg::NoSelect);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x01F, "ITEM_LIST type") && ok;
    ok      = expectEqualUInt(packet.getSize(), itemListFullPacketSize, "ITEM_LIST size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 16>{ 0x1F, 0x08, 0xEF, 0xBE, 0x04, 0x03, 0x02, 0x01, 0x34, 0x12, 0x0B, 0x22, 0x0F, 0x00, 0x00, 0x00 }, "constructor bytes") && ok;
    ok      = expectZeroTail(packet, itemListFullPacketSize, "constructor tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
