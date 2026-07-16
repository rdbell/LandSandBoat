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

#include "test_s2c_equip_list_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"
#include "map/item_container.h"
#include "map/packets/s2c/0x050_equip_list.h"

namespace
{

constexpr auto equipListPropertyItemIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIP_LIST::PacketData, PropertyItemIndex);
constexpr auto equipListEquipKindOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIP_LIST::PacketData, EquipKind);
constexpr auto equipListCategoryOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIP_LIST::PacketData, Category);
constexpr auto equipListPadding00Offset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIP_LIST::PacketData, padding00);
constexpr auto equipListPacketDataSize          = sizeof(GP_SERV_COMMAND_EQUIP_LIST::PacketData);
constexpr auto equipListFullPacketSize          = sizeof(GP_SERV_HEADER) + equipListPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EQUIP_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EQUIP_LIST packet self-test failed: " << label << " got";
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
            std::cerr << "s2c EQUIP_LIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(equipListPacketDataSize, 4, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(equipListFullPacketSize, 8, "full packet size") && ok;
    ok      = expectEqualUInt(equipListPropertyItemIndexOffset, 4, "PropertyItemIndex offset") && ok;
    ok      = expectEqualUInt(equipListEquipKindOffset, 5, "EquipKind offset") && ok;
    ok      = expectEqualUInt(equipListCategoryOffset, 6, "Category offset") && ok;
    ok      = expectEqualUInt(equipListPadding00Offset, 7, "padding00 offset") && ok;
    ok      = expectEqualUInt(SLOT_HANDS, 6, "SLOT_HANDS") && ok;
    ok      = expectEqualUInt(SLOT_RING2, 14, "SLOT_RING2") && ok;
    ok      = expectEqualUInt(LOC_WARDROBE3, 11, "LOC_WARDROBE3") && ok;
    return ok;
}

auto testScalarConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_EQUIP_LIST(0x12, SLOT_HANDS, LOC_WARDROBE3);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x050, "EQUIP_LIST type") && ok;
    ok      = expectEqualUInt(packet.getSize(), equipListFullPacketSize, "EQUIP_LIST size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 8>{ 0x50, 0x04, 0xEF, 0xBE, 0x12, 0x06, 0x0B, 0x00 }, "scalar constructor bytes") && ok;
    ok      = expectZeroTail(packet, equipListFullPacketSize, "scalar constructor tail") && ok;
    return ok;
}

auto testItemLocationConstructor() -> bool
{
    const auto loc    = ItemLocation{ LOC_WARDROBE3, 0x22 };
    auto       packet = GP_SERV_COMMAND_EQUIP_LIST(loc, SLOT_RING2);

    bool ok = true;
    ok      = expectBytes(packet, equipListPropertyItemIndexOffset, std::array<uint8, 4>{ 0x22, 0x0E, 0x0B, 0x00 }, "ItemLocation constructor bytes") && ok;
    ok      = expectZeroTail(packet, equipListFullPacketSize, "ItemLocation constructor tail") && ok;
    return ok;
}

auto testConstructorsShapeEquivalentPayloads() -> bool
{
    const auto loc      = ItemLocation{ LOC_WARDROBE3, 0x22 };
    auto       scalar   = GP_SERV_COMMAND_EQUIP_LIST(0x22, SLOT_RING2, LOC_WARDROBE3);
    auto       location = GP_SERV_COMMAND_EQUIP_LIST(loc, SLOT_RING2);
    const auto expected = std::array<uint8, 4>{ 0x22, 0x0E, 0x0B, 0x00 };

    bool ok = true;
    ok      = expectBytes(scalar, equipListPropertyItemIndexOffset, expected, "scalar equivalent payload") && ok;
    ok      = expectBytes(location, equipListPropertyItemIndexOffset, expected, "ItemLocation equivalent payload") && ok;
    return ok;
}

} // namespace

auto runS2CEquipListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testScalarConstructor() && ok;
    ok      = testItemLocationConstructor() && ok;
    ok      = testConstructorsShapeEquivalentPayloads() && ok;
    return ok;
}
