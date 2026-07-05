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

#include "test_s2c_item_subcontainer_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/item_container.h"
#include "map/packets/s2c/0x026_item_subcontainer.h"

namespace
{

constexpr auto subcontainerIsUsedOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, is_used);
constexpr auto subcontainerContainerOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, container);
constexpr auto subcontainerIndexOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, index);
constexpr auto subcontainerUnknown00Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, unknown00);
constexpr auto subcontainerUnknown01Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, unknown01);
constexpr auto subcontainerRaceHairOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_race_hair);
constexpr auto subcontainerHeadOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_head);
constexpr auto subcontainerBodyOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_body);
constexpr auto subcontainerHandsOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_hands);
constexpr auto subcontainerLegsOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_legs);
constexpr auto subcontainerFeetOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_feet);
constexpr auto subcontainerMainOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_main);
constexpr auto subcontainerSubOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_sub);
constexpr auto subcontainerRangeOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, model_id_range);
constexpr auto subcontainerRaceOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, race);
constexpr auto subcontainerPoseOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData, pose);
constexpr auto subcontainerPacketDataSize        = sizeof(GP_SERV_COMMAND_ITEM_SUBCONTAINER::PacketData);
constexpr auto subcontainerUnroundedPacketSize   = sizeof(GP_SERV_HEADER) + subcontainerPacketDataSize;
constexpr auto subcontainerRoundedPacketSize     = 32U;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ITEM_SUBCONTAINER packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ITEM_SUBCONTAINER packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ITEM_SUBCONTAINER packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(subcontainerPacketDataSize, 26, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(subcontainerUnroundedPacketSize, 30, "unrounded packet size") && ok;
    ok      = expectEqualUInt(subcontainerIsUsedOffset, 4, "is_used offset") && ok;
    ok      = expectEqualUInt(subcontainerContainerOffset, 5, "container offset") && ok;
    ok      = expectEqualUInt(subcontainerIndexOffset, 6, "index offset") && ok;
    ok      = expectEqualUInt(subcontainerUnknown00Offset, 7, "unknown00 offset") && ok;
    ok      = expectEqualUInt(subcontainerUnknown01Offset, 8, "unknown01 offset") && ok;
    ok      = expectEqualUInt(subcontainerRaceHairOffset, 10, "model_id_race_hair offset") && ok;
    ok      = expectEqualUInt(subcontainerHeadOffset, 12, "model_id_head offset") && ok;
    ok      = expectEqualUInt(subcontainerBodyOffset, 14, "model_id_body offset") && ok;
    ok      = expectEqualUInt(subcontainerHandsOffset, 16, "model_id_hands offset") && ok;
    ok      = expectEqualUInt(subcontainerLegsOffset, 18, "model_id_legs offset") && ok;
    ok      = expectEqualUInt(subcontainerFeetOffset, 20, "model_id_feet offset") && ok;
    ok      = expectEqualUInt(subcontainerMainOffset, 22, "model_id_main offset") && ok;
    ok      = expectEqualUInt(subcontainerSubOffset, 24, "model_id_sub offset") && ok;
    ok      = expectEqualUInt(subcontainerRangeOffset, 26, "model_id_range offset") && ok;
    ok      = expectEqualUInt(subcontainerRaceOffset, 28, "race offset") && ok;
    ok      = expectEqualUInt(subcontainerPoseOffset, 29, "pose offset") && ok;
    return ok;
}

auto testEmptyConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_ITEM_SUBCONTAINER(LOC_STORAGE, 0x22);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x026, "ITEM_SUBCONTAINER type") && ok;
    ok      = expectEqualUInt(packet.getSize(), subcontainerRoundedPacketSize, "ITEM_SUBCONTAINER size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 10>{ 0x26, 0x10, 0xEF, 0xBE, 0x00, 0x02, 0x22, 0x00, 0x00, 0x00 }, "empty constructor prefix") && ok;
    ok      = expectZeroTail(packet, 10, "empty constructor tail") && ok;
    return ok;
}

auto testMannequinModelConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_ITEM_SUBCONTAINER(LOC_STORAGE, 0x33, 0x0011, 0x0022, 0x0033, 0x0044, 0x0055, 0x0066, 0x0077, 0x0088);

    const auto expectedPayload = std::array<uint8, 26>{
        0x01, 0x02, 0x33, 0x00, 0x00, 0x00,
        0x01, 0x00,
        0x11, 0x10,
        0x22, 0x20,
        0x33, 0x30,
        0x44, 0x40,
        0x55, 0x50,
        0x66, 0x60,
        0x77, 0x70,
        0x88, 0x80,
        0x00, 0x00,
    };

    bool ok = true;
    ok      = expectBytes(packet, subcontainerIsUsedOffset, expectedPayload, "mannequin constructor payload") && ok;
    ok      = expectZeroTail(packet, subcontainerRoundedPacketSize, "mannequin constructor tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemSubcontainerPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEmptyConstructor() && ok;
    ok      = testMannequinModelConstructor() && ok;
    return ok;
}
