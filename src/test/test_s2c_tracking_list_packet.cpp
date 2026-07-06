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

#include "test_s2c_tracking_list_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/base_entity.h"
#include "entities/battle_entity.h"
#include "entities/char_entity.h"
#include "map/packets/s2c/0x0f4_tracking_list.h"

namespace
{

constexpr auto trackingListBitfieldOffset      = sizeof(GP_SERV_HEADER);
constexpr auto trackingListXOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_LIST::PacketData, x);
constexpr auto trackingListZOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_LIST::PacketData, z);
constexpr auto trackingListNameOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_LIST::PacketData, sName);
constexpr auto trackingListNameSize            = sizeof(GP_SERV_COMMAND_TRACKING_LIST::PacketData::sName);
constexpr auto trackingListPacketDataSize      = sizeof(GP_SERV_COMMAND_TRACKING_LIST::PacketData);
constexpr auto trackingListPacketSize          = sizeof(GP_SERV_HEADER) + trackingListPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TRACKING_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TRACKING_LIST packet self-test failed: " << label << " got";
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

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < end; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c TRACKING_LIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

void populateChar(CCharEntity& character, float x, float z)
{
    character.loc.p.x = x;
    character.loc.p.z = z;
}

void populateTarget(CBaseEntity& entity, std::uint16_t targid, ENTITYTYPE objtype, float x, float z)
{
    entity.targid  = targid;
    entity.objtype = objtype;
    entity.loc.p.x = x;
    entity.loc.p.z = z;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(trackingListPacketDataSize, 24, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(trackingListPacketSize, 28, "packet size") && ok;
    ok      = expectEqualUInt(trackingListBitfieldOffset, 4, "bitfield offset") && ok;
    ok      = expectEqualUInt(trackingListXOffset, 8, "x offset") && ok;
    ok      = expectEqualUInt(trackingListZOffset, 10, "z offset") && ok;
    ok      = expectEqualUInt(trackingListNameOffset, 12, "sName offset") && ok;
    ok      = expectEqualUInt(trackingListNameSize, 16, "sName size") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_TRACKING_LIST), 0x0F4, "packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(TYPE_PC), 1, "TYPE_PC") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(TYPE_NPC), 2, "TYPE_NPC") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(TYPE_MOB), 4, "TYPE_MOB") && ok;
    return ok;
}

auto testMobConstructor() -> bool
{
    auto character = CCharEntity{};
    auto target    = CBattleEntity{};
    populateChar(character, 10.75f, -20.25f);
    populateTarget(target, 0x5566, TYPE_MOB, -4.25f, 13.5f);
    target.SetMLevel(0x22);
    auto packet = GP_SERV_COMMAND_TRACKING_LIST(&character, &target);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 28>{
        0xF4, 0x0E, 0xEF, 0xBE,
        0x66, 0x55, 0x22, 0x02,
        0xF1, 0xFF,
        0x21, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0F4, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), trackingListPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, expected, "encoded mob packet") && ok;
    ok      = expectZeroRange(packet, trackingListPacketSize, PACKET_SIZE, "mob tail") && ok;
    return ok;
}

auto testNonMobConstructor() -> bool
{
    auto character = CCharEntity{};
    auto target    = CCharEntity{};
    populateChar(character, -100.5f, 7.25f);
    populateTarget(target, 0x7788, TYPE_NPC, -98.0f, 1.0f);
    target.SetMLevel(0x44);
    auto packet = GP_SERV_COMMAND_TRACKING_LIST(&character, &target);

    bool ok = true;
    ok      = expectBytes(packet, trackingListBitfieldOffset, std::array<uint8, 4>{ 0x88, 0x77, 0x00, 0x01 }, "non-mob bitfield") && ok;
    ok      = expectBytes(packet, trackingListXOffset, std::array<uint8, 2>{ 0x02, 0x00 }, "non-mob x") && ok;
    ok      = expectBytes(packet, trackingListZOffset, std::array<uint8, 2>{ 0xFA, 0xFF }, "non-mob z") && ok;
    ok      = expectZeroRange(packet, trackingListNameOffset, trackingListNameOffset + trackingListNameSize, "non-mob sName") && ok;
    return ok;
}

} // namespace

auto runS2CTrackingListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testMobConstructor() && ok;
    ok      = testNonMobConstructor() && ok;
    return ok;
}
