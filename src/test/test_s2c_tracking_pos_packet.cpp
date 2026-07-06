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

#include "test_s2c_tracking_pos_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/base_entity.h"
#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x0f5_tracking_pos.h"

namespace
{

constexpr auto trackingPosXOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_POS::PacketData, x);
constexpr auto trackingPosYOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_POS::PacketData, y);
constexpr auto trackingPosZOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_POS::PacketData, z);
constexpr auto trackingPosLevelOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_POS::PacketData, Level);
constexpr auto trackingPosUnusedOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_POS::PacketData, unused);
constexpr auto trackingPosActIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_POS::PacketData, ActIndex);
constexpr auto trackingPosStateOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_POS::PacketData, State);
constexpr auto trackingPosPacketDataSize   = sizeof(GP_SERV_COMMAND_TRACKING_POS::PacketData);
constexpr auto trackingPosPacketSize       = sizeof(GP_SERV_HEADER) + trackingPosPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TRACKING_POS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TRACKING_POS packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TRACKING_POS packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

void populateEntity(CBaseEntity& entity, std::uint16_t targid, STATUS_TYPE status)
{
    entity.targid  = targid;
    entity.status  = status;
    entity.loc.p.x = 1.5f;
    entity.loc.p.y = -2.25f;
    entity.loc.p.z = 3.75f;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_TRACKING_POS_STATE), 1, "sizeof(GP_TRACKING_POS_STATE)") && ok;
    ok      = expectEqualUInt(trackingPosPacketDataSize, 20, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(trackingPosPacketSize, 24, "packet size") && ok;
    ok      = expectEqualUInt(trackingPosXOffset, 4, "x offset") && ok;
    ok      = expectEqualUInt(trackingPosYOffset, 8, "y offset") && ok;
    ok      = expectEqualUInt(trackingPosZOffset, 12, "z offset") && ok;
    ok      = expectEqualUInt(trackingPosLevelOffset, 16, "Level offset") && ok;
    ok      = expectEqualUInt(trackingPosUnusedOffset, 17, "unused offset") && ok;
    ok      = expectEqualUInt(trackingPosActIndexOffset, 18, "ActIndex offset") && ok;
    ok      = expectEqualUInt(trackingPosStateOffset, 20, "State offset") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_TRACKING_POS), 0x0F5, "packet id") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_TRACKING_POS_STATE::None), 0, "None enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_TRACKING_POS_STATE::Start), 1, "Start enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_TRACKING_POS_STATE::Lose), 2, "Lose enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_TRACKING_POS_STATE::End), 3, "End enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(STATUS_TYPE::NORMAL), 0, "STATUS_TYPE::NORMAL") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(STATUS_TYPE::DISAPPEAR), 2, "STATUS_TYPE::DISAPPEAR") && ok;
    return ok;
}

auto testNormalConstructor() -> bool
{
    auto entity = CCharEntity{};
    populateEntity(entity, 0x5566, STATUS_TYPE::NORMAL);
    auto packet = GP_SERV_COMMAND_TRACKING_POS(&entity);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 24>{
        0xF5, 0x0C, 0xEF, 0xBE,
        0x00, 0x00, 0xC0, 0x3F,
        0x00, 0x00, 0x10, 0xC0,
        0x00, 0x00, 0x70, 0x40,
        0x01, 0x00, 0x66, 0x55,
        0x01, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0F5, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), trackingPosPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, expected, "encoded normal packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "normal tail") && ok;
    return ok;
}

auto testDisappearConstructor() -> bool
{
    auto entity = CCharEntity{};
    populateEntity(entity, 0x7788, STATUS_TYPE::DISAPPEAR);
    auto packet = GP_SERV_COMMAND_TRACKING_POS(&entity);

    bool ok = true;
    ok      = expectBytes(packet, trackingPosActIndexOffset, std::array<uint8, 2>{ 0x88, 0x77 }, "ActIndex") && ok;
    ok      = expectBytes(packet, trackingPosStateOffset, std::array<uint8, 1>{ 0x02 }, "Lose state") && ok;
    return ok;
}

} // namespace

auto runS2CTrackingPosPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testNormalConstructor() && ok;
    ok      = testDisappearConstructor() && ok;
    return ok;
}
