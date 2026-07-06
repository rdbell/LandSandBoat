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

#include "test_s2c_equipset_valid_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x116_equipset_valid.h"

namespace
{

constexpr auto equipSetValidItemCount        = 17U;
constexpr auto equipSetValidItemSize         = 4U;
constexpr auto equipSetValidItemsOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIPSET_VALID::PacketData, Items);
constexpr auto equipSetValidPacketDataSize   = sizeof(GP_SERV_COMMAND_EQUIPSET_VALID::PacketData);
constexpr auto equipSetValidPacketSize       = sizeof(GP_SERV_HEADER) + equipSetValidPacketDataSize;
constexpr auto requestItemFlagsOffset        = 0U;
constexpr auto requestItemIndexOffset        = 1U;
constexpr auto requestItemItemNoOffset       = 2U;

using PacketDataBytes = std::array<std::uint8_t, sizeof(GP_SERV_COMMAND_EQUIPSET_VALID::PacketData)>;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EQUIPSET_VALID packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EQUIPSET_VALID packet self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
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

auto requestItemOffset(std::size_t index) -> std::size_t
{
    return index * equipSetValidItemSize;
}

auto packetItemOffset(std::size_t index) -> std::size_t
{
    return equipSetValidItemsOffset + requestItemOffset(index);
}

auto itemFlags(std::uint8_t hasItem, std::uint8_t removeItem, std::uint8_t category) -> std::uint8_t
{
    return static_cast<std::uint8_t>((hasItem & 0x01U) | ((removeItem & 0x01U) << 1U) | ((category & 0x3FU) << 2U));
}

auto encodedPacketDataBytes(const GP_SERV_COMMAND_EQUIPSET_VALID::PacketData& packetData) -> PacketDataBytes
{
    auto bytes = PacketDataBytes{};
    std::memcpy(bytes.data(), &packetData, bytes.size());
    return bytes;
}

auto makePacketData() -> GP_SERV_COMMAND_EQUIPSET_VALID::PacketData
{
    auto packetData = GP_SERV_COMMAND_EQUIPSET_VALID::PacketData{};

    packetData.Items[0].HasItemFlg    = 1;
    packetData.Items[0].RemoveItemFlg = 0;
    packetData.Items[0].Category      = 0x03;
    packetData.Items[0].ItemIndex     = 0x44;
    packetData.Items[0].ItemNo        = 0x1234;

    packetData.Items[1].HasItemFlg    = 0;
    packetData.Items[1].RemoveItemFlg = 1;
    packetData.Items[1].Category      = 0x15;
    packetData.Items[1].ItemIndex     = 0x55;
    packetData.Items[1].ItemNo        = 0x2001;

    packetData.Items[16].HasItemFlg    = 1;
    packetData.Items[16].RemoveItemFlg = 1;
    packetData.Items[16].Category      = 0x3F;
    packetData.Items[16].ItemIndex     = 0x66;
    packetData.Items[16].ItemNo        = 0xBEEF;

    return packetData;
}

auto makeExpectedPacketDataBytes() -> PacketDataBytes
{
    auto bytes = PacketDataBytes{};

    bytes[requestItemOffset(0) + requestItemFlagsOffset]  = itemFlags(1, 0, 0x03);
    bytes[requestItemOffset(0) + requestItemIndexOffset]  = 0x44;
    bytes[requestItemOffset(0) + requestItemItemNoOffset] = 0x34;
    bytes[requestItemOffset(0) + requestItemItemNoOffset + 1U] = 0x12;

    bytes[requestItemOffset(1) + requestItemFlagsOffset]  = itemFlags(0, 1, 0x15);
    bytes[requestItemOffset(1) + requestItemIndexOffset]  = 0x55;
    bytes[requestItemOffset(1) + requestItemItemNoOffset] = 0x01;
    bytes[requestItemOffset(1) + requestItemItemNoOffset + 1U] = 0x20;

    bytes[requestItemOffset(16) + requestItemFlagsOffset]  = itemFlags(1, 1, 0x3F);
    bytes[requestItemOffset(16) + requestItemIndexOffset]  = 0x66;
    bytes[requestItemOffset(16) + requestItemItemNoOffset] = 0xEF;
    bytes[requestItemOffset(16) + requestItemItemNoOffset + 1U] = 0xBE;

    return bytes;
}

auto testLayoutAndMetadata() -> bool
{
    bool ok = true;

    const auto packetData = GP_SERV_COMMAND_EQUIPSET_VALID::PacketData{};
    const auto item       = equipsetrequestitem_t{};

    ok = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_EQUIPSET_VALID), 0x116, "packet id") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok = expectEqualUInt(sizeof(equipsetrequestitem_t), equipSetValidItemSize, "equipsetrequestitem_t sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_EQUIPSET_VALID::PacketData::Items) / sizeof(GP_SERV_COMMAND_EQUIPSET_VALID::PacketData::Items[0]),
                         equipSetValidItemCount,
                         "Items count") && ok;
    ok = expectEqualUInt(sizeof(packetData.Items) / sizeof(packetData.Items[0]), equipSetValidItemCount, "packet data Items count") && ok;
    ok = expectEqualUInt(equipSetValidPacketDataSize, 68, "sizeof(PacketData)") && ok;
    ok = expectEqualUInt(equipSetValidPacketSize, 72, "packet size") && ok;
    ok = expectEqualUInt(equipSetValidItemsOffset, 4, "Items offset") && ok;
    ok = expectEqualUInt(packetItemOffset(0), 4, "Items[0] packet offset") && ok;
    ok = expectEqualUInt(packetItemOffset(1), 8, "Items[1] packet offset") && ok;
    ok = expectEqualUInt(packetItemOffset(16), 68, "Items[16] packet offset") && ok;
    ok = expectEqualUInt(offsetof(equipsetrequestitem_t, ItemIndex), requestItemIndexOffset, "ItemIndex offset") && ok;
    ok = expectEqualUInt(offsetof(equipsetrequestitem_t, ItemNo), requestItemItemNoOffset, "ItemNo offset") && ok;
    ok = expectEqualUInt(sizeof(item.ItemIndex), 1, "ItemIndex size") && ok;
    ok = expectEqualUInt(sizeof(item.ItemNo), 2, "ItemNo size") && ok;

    return ok;
}

auto testPacketDataEncoding() -> bool
{
    bool ok = true;
    ok      = expectBytes(encodedPacketDataBytes(makePacketData()), makeExpectedPacketDataBytes(), "PacketData bytes") && ok;
    ok      = expectEqualUInt(itemFlags(1, 1, 0x3F), 0xFF, "all bitfield flags") && ok;
    ok      = expectEqualUInt(itemFlags(0, 0, 0x15), 0x54, "category bitfield flags") && ok;
    return ok;
}

} // namespace

auto runS2CEquipSetValidPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndMetadata() && ok;
    ok      = testPacketDataEncoding() && ok;
    return ok;
}
