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

#include "test_s2c_item_num_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/item_container.h"
#include "map/packets/s2c/0x01e_item_num.h"

namespace
{

constexpr auto itemNumItemNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_NUM::PacketData, ItemNum);
constexpr auto itemNumCategoryOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_NUM::PacketData, Category);
constexpr auto itemNumItemIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_NUM::PacketData, ItemIndex);
constexpr auto itemNumLockFlgOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_NUM::PacketData, LockFlg);
constexpr auto itemNumPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_NUM::PacketData, padding00);
constexpr auto itemNumPacketDataSize   = sizeof(GP_SERV_COMMAND_ITEM_NUM::PacketData);
constexpr auto itemNumFullPacketSize   = sizeof(GP_SERV_HEADER) + itemNumPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ITEM_NUM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ITEM_NUM packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ITEM_NUM packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(itemNumPacketDataSize, 8, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(itemNumFullPacketSize, 12, "full packet size") && ok;
    ok      = expectEqualUInt(itemNumItemNumOffset, 4, "ItemNum offset") && ok;
    ok      = expectEqualUInt(itemNumCategoryOffset, 8, "Category offset") && ok;
    ok      = expectEqualUInt(itemNumItemIndexOffset, 9, "ItemIndex offset") && ok;
    ok      = expectEqualUInt(itemNumLockFlgOffset, 10, "LockFlg offset") && ok;
    ok      = expectEqualUInt(itemNumPadding00Offset, 11, "padding00 offset") && ok;
    ok      = expectEqualUInt(LOC_WARDROBE3, 11, "LOC_WARDROBE3") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_ITEM_NUM(LOC_WARDROBE3, 0x22, 0x01020304);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x01E, "ITEM_NUM type") && ok;
    ok      = expectEqualUInt(packet.getSize(), itemNumFullPacketSize, "ITEM_NUM size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 12>{ 0x1E, 0x06, 0xEF, 0xBE, 0x04, 0x03, 0x02, 0x01, 0x0B, 0x22, 0x00, 0x00 }, "constructor bytes") && ok;
    ok      = expectZeroTail(packet, itemNumFullPacketSize, "constructor tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemNumPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
