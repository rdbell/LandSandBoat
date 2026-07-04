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

#include "test_s2c_default_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x03b_eventmes.h"
#include "map/packets/s2c/0x0ab_feat_data.h"
#include "map/packets/s2c/0x0de_group_solicit_no.h"
#include "map/packets/s2c/0x117_equipset_res.h"

namespace
{

constexpr auto eventMesUniqueNoOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTMES::PacketData, UniqueNo);
constexpr auto eventMesActIndexOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTMES::PacketData, ActIndex);
constexpr auto eventMesNumberOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTMES::PacketData, Number);
constexpr auto eventMesPacketSize          = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_EVENTMES::PacketData);
constexpr auto featDataTblOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FEAT_DATA::PacketData, FeatDataTbl);
constexpr auto featDataPacketSize          = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_FEAT_DATA::PacketData);
constexpr auto groupSolicitNoReasonOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_SOLICIT_NO::PacketData, Reason);
constexpr auto groupSolicitNoPaddingOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_SOLICIT_NO::PacketData, padding05);
constexpr auto groupSolicitNoPacketSize    = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_GROUP_SOLICIT_NO::PacketData);
constexpr auto equipSetResCountOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIPSET_RES::PacketData, Count);
constexpr auto equipSetResPaddingOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIPSET_RES::PacketData, padding05);
constexpr auto equipSetResItemsChangedOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIPSET_RES::PacketData, ItemsChanged);
constexpr auto equipSetResItemsEquippedOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EQUIPSET_RES::PacketData, ItemsEquipped);
constexpr auto equipSetResPacketSize       = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_EQUIPSET_RES::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c default packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c default packet self-test failed: " << label << " got";
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
            std::cerr << "s2c default packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    return expectZeroRange(packet, offset, PACKET_SIZE, label);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_EVENTMES::PacketData), 8, "EVENTMES sizeof(PacketData)") && ok;
    ok = expectEqualUInt(eventMesPacketSize, 12, "EVENTMES packet size") && ok;
    ok = expectEqualUInt(eventMesUniqueNoOffset, 4, "EVENTMES UniqueNo offset") && ok;
    ok = expectEqualUInt(eventMesActIndexOffset, 8, "EVENTMES ActIndex offset") && ok;
    ok = expectEqualUInt(eventMesNumberOffset, 10, "EVENTMES Number offset") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_FEAT_DATA::PacketData), 20, "FEAT_DATA sizeof(PacketData)") && ok;
    ok = expectEqualUInt(featDataPacketSize, 24, "FEAT_DATA packet size") && ok;
    ok = expectEqualUInt(featDataTblOffset, 4, "FEAT_DATA FeatDataTbl offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_FEAT_DATA::PacketData::FeatDataTbl), 20, "FEAT_DATA FeatDataTbl size") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_GROUP_SOLICIT_NO::PacketData), 4, "GROUP_SOLICIT_NO sizeof(PacketData)") && ok;
    ok = expectEqualUInt(groupSolicitNoPacketSize, 8, "GROUP_SOLICIT_NO packet size") && ok;
    ok = expectEqualUInt(groupSolicitNoReasonOffset, 4, "GROUP_SOLICIT_NO Reason offset") && ok;
    ok = expectEqualUInt(groupSolicitNoPaddingOffset, 5, "GROUP_SOLICIT_NO padding05 offset") && ok;

    ok = expectEqualUInt(sizeof(equipsetitem_t), 4, "equipsetitem_t sizeof") && ok;
    ok = expectEqualUInt(offsetof(equipsetitem_t, ItemIndex), 0, "equipsetitem_t ItemIndex offset") && ok;
    ok = expectEqualUInt(offsetof(equipsetitem_t, EquipKind), 1, "equipsetitem_t EquipKind offset") && ok;
    ok = expectEqualUInt(offsetof(equipsetitem_t, Category), 2, "equipsetitem_t Category offset") && ok;
    ok = expectEqualUInt(offsetof(equipsetitem_t, padding03), 3, "equipsetitem_t padding03 offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_EQUIPSET_RES::PacketData), 132, "EQUIPSET_RES sizeof(PacketData)") && ok;
    ok = expectEqualUInt(equipSetResPacketSize, 136, "EQUIPSET_RES packet size") && ok;
    ok = expectEqualUInt(equipSetResCountOffset, 4, "EQUIPSET_RES Count offset") && ok;
    ok = expectEqualUInt(equipSetResPaddingOffset, 5, "EQUIPSET_RES padding05 offset") && ok;
    ok = expectEqualUInt(equipSetResItemsChangedOffset, 8, "EQUIPSET_RES ItemsChanged offset") && ok;
    ok = expectEqualUInt(equipSetResItemsEquippedOffset, 72, "EQUIPSET_RES ItemsEquipped offset") && ok;
    return ok;
}

auto testEventMesConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_EVENTMES();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0x3B, 0x06, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x03B, "EVENTMES type") && ok;
    ok      = expectEqualUInt(packet.getSize(), eventMesPacketSize, "EVENTMES size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded EVENTMES prefix") && ok;
    ok      = expectZeroTail(packet, eventMesPacketSize, "EVENTMES tail") && ok;
    return ok;
}

auto testFeatDataConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_FEAT_DATA();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0xAB, 0x0C, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0AB, "FEAT_DATA type") && ok;
    ok      = expectEqualUInt(packet.getSize(), featDataPacketSize, "FEAT_DATA size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded FEAT_DATA prefix") && ok;
    ok      = expectZeroRange(packet, featDataTblOffset, featDataPacketSize, "FEAT_DATA payload") && ok;
    ok      = expectZeroTail(packet, featDataPacketSize, "FEAT_DATA tail") && ok;
    return ok;
}

auto testGroupSolicitNoConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_GROUP_SOLICIT_NO();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 8>{
        0xDE, 0x04, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0DE, "GROUP_SOLICIT_NO type") && ok;
    ok      = expectEqualUInt(packet.getSize(), groupSolicitNoPacketSize, "GROUP_SOLICIT_NO size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded GROUP_SOLICIT_NO prefix") && ok;
    ok      = expectZeroTail(packet, groupSolicitNoPacketSize, "GROUP_SOLICIT_NO tail") && ok;
    return ok;
}

auto testEquipSetResConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_EQUIPSET_RES();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0x17, 0x45, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x117, "EQUIPSET_RES type") && ok;
    ok      = expectEqualUInt(packet.getSize(), equipSetResPacketSize, "EQUIPSET_RES size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded EQUIPSET_RES prefix") && ok;
    ok      = expectZeroRange(packet, equipSetResCountOffset, equipSetResPacketSize, "EQUIPSET_RES payload") && ok;
    ok      = expectZeroTail(packet, equipSetResPacketSize, "EQUIPSET_RES tail") && ok;
    return ok;
}

} // namespace

auto runS2CDefaultPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEventMesConstructor() && ok;
    ok      = testFeatDataConstructor() && ok;
    ok      = testGroupSolicitNoConstructor() && ok;
    ok      = testEquipSetResConstructor() && ok;
    return ok;
}
