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

#include "test_s2c_item_max_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/item_container.h"
#include "map/packets/s2c/0x01c_item_max.h"

namespace
{

constexpr auto itemMaxItemNumOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_MAX::PacketData, ItemNum);
constexpr auto itemMaxPadding00Offset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_MAX::PacketData, padding00);
constexpr auto itemMaxItemNum2Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_MAX::PacketData, ItemNum2);
constexpr auto itemMaxPadding01Offset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_MAX::PacketData, padding01);
constexpr auto itemMaxPacketDataSize    = sizeof(GP_SERV_COMMAND_ITEM_MAX::PacketData);
constexpr auto itemMaxFullPacketSize    = sizeof(GP_SERV_HEADER) + itemMaxPacketDataSize;
constexpr auto itemMaxContainerCount    = MAX_CONTAINER_ID;
constexpr auto itemMaxPadding00Size     = sizeof(GP_SERV_COMMAND_ITEM_MAX::PacketData::padding00);
constexpr auto itemMaxPadding01Size     = sizeof(GP_SERV_COMMAND_ITEM_MAX::PacketData::padding01);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ITEM_MAX packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ITEM_MAX packet self-test failed: " << label << " got";
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

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t size, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < offset + size; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c ITEM_MAX packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    return expectZeroRange(packet, offset, PACKET_SIZE - offset, label);
}

void sizeCharacter(CCharEntity& character)
{
    character.status      = STATUS_TYPE::NORMAL;
    character.health.hp   = 1;
    character.health.maxhp = 1;
    character.updateCharVarCache("mog-locker-expiry-timestamp", 0);
    character.updateCharVarCache("mog-locker-access-type", 0);

    for (uint8 location = 0; location < MAX_CONTAINER_ID; ++location)
    {
        auto* storage = character.getStorage(location);
        storage->AddBuff(static_cast<int8>(20 + location));
        storage->SetSize(static_cast<uint8>(location));
    }
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(itemMaxPacketDataSize, 96, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(itemMaxFullPacketSize, 100, "full packet size") && ok;
    ok      = expectEqualUInt(itemMaxItemNumOffset, 4, "ItemNum offset") && ok;
    ok      = expectEqualUInt(itemMaxPadding00Offset, 22, "padding00 offset") && ok;
    ok      = expectEqualUInt(itemMaxItemNum2Offset, 36, "ItemNum2 offset") && ok;
    ok      = expectEqualUInt(itemMaxPadding01Offset, 72, "padding01 offset") && ok;
    ok      = expectEqualUInt(itemMaxContainerCount, 18, "container count") && ok;
    ok      = expectEqualUInt(itemMaxPadding00Size, 14, "padding00 size") && ok;
    ok      = expectEqualUInt(itemMaxPadding01Size, 28, "padding01 size") && ok;
    ok      = expectEqualUInt(LOC_MOGLOCKER, 4, "LOC_MOGLOCKER") && ok;
    ok      = expectEqualUInt(LOC_RECYCLEBIN, 17, "LOC_RECYCLEBIN") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto character = CCharEntity{};
    sizeCharacter(character);
    auto packet = GP_SERV_COMMAND_ITEM_MAX(&character);
    packet.setSequence(0xBEEF);

    const auto expectedHeader = std::array<uint8, 4>{ 0x1C, 0x32, 0xEF, 0xBE };
    const auto expectedItemNum = std::array<uint8, 18>{
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
    };
    const auto expectedItemNum2 = std::array<uint8, 36>{
        0x15, 0x00, 0x16, 0x00, 0x17, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x1A, 0x00,
        0x1B, 0x00, 0x1C, 0x00, 0x1D, 0x00,
        0x1E, 0x00, 0x1F, 0x00, 0x20, 0x00,
        0x21, 0x00, 0x22, 0x00, 0x23, 0x00,
        0x24, 0x00, 0x25, 0x00, 0x26, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x01C, "ITEM_MAX type") && ok;
    ok      = expectEqualUInt(packet.getSize(), itemMaxFullPacketSize, "ITEM_MAX size") && ok;
    ok      = expectBytes(packet, 0, expectedHeader, "header bytes") && ok;
    ok      = expectBytes(packet, itemMaxItemNumOffset, expectedItemNum, "ItemNum bytes") && ok;
    ok      = expectZeroRange(packet, itemMaxPadding00Offset, itemMaxPadding00Size, "padding00") && ok;
    ok      = expectBytes(packet, itemMaxItemNum2Offset, expectedItemNum2, "ItemNum2 bytes") && ok;
    ok      = expectZeroRange(packet, itemMaxPadding01Offset, itemMaxPadding01Size, "padding01") && ok;
    ok      = expectZeroTail(packet, itemMaxFullPacketSize, "tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemMaxPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
