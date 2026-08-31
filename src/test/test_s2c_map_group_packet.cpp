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

#include "test_s2c_map_group_packet.h"

#include "test/omega_self_test_registry.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0a0_map_group.h"

namespace
{

using MapGroupPacket = GP_SERV_COMMAND_MAP_GROUP;

constexpr auto mapGroupUniqueIDOffset  = sizeof(GP_SERV_HEADER) + offsetof(MapGroupPacket::PacketData, UniqueID);
constexpr auto mapGroupZoneOffset      = sizeof(GP_SERV_HEADER) + offsetof(MapGroupPacket::PacketData, zone);
constexpr auto mapGroupPadding0AOffset = sizeof(GP_SERV_HEADER) + offsetof(MapGroupPacket::PacketData, padding0A);
constexpr auto mapGroupXOffset         = sizeof(GP_SERV_HEADER) + offsetof(MapGroupPacket::PacketData, x);
constexpr auto mapGroupYOffset         = sizeof(GP_SERV_HEADER) + offsetof(MapGroupPacket::PacketData, y);
constexpr auto mapGroupZOffset         = sizeof(GP_SERV_HEADER) + offsetof(MapGroupPacket::PacketData, z);
constexpr auto mapGroupPacketDataSize  = sizeof(MapGroupPacket::PacketData);
constexpr auto mapGroupPacketSize      = sizeof(GP_SERV_HEADER) + mapGroupPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MAP_GROUP packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <typename T, std::size_t Size>
auto expectStructBytes(const T& value, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = reinterpret_cast<const uint8*>(&value);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c MAP_GROUP packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto valueByte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(valueByte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MAP_GROUP), 0x0A0, "MAP_GROUP packet id") && ok;
    ok      = expectEqualUInt(mapGroupPacketDataSize, 20, "sizeof(MAP_GROUP::PacketData)") && ok;
    ok      = expectEqualUInt(mapGroupPacketSize, 24, "MAP_GROUP packet size") && ok;
    ok      = expectEqualUInt(mapGroupUniqueIDOffset, 4, "UniqueID offset") && ok;
    ok      = expectEqualUInt(mapGroupZoneOffset, 8, "zone offset") && ok;
    ok      = expectEqualUInt(mapGroupPadding0AOffset, 10, "padding0A offset") && ok;
    ok      = expectEqualUInt(mapGroupXOffset, 12, "x offset") && ok;
    ok      = expectEqualUInt(mapGroupYOffset, 16, "y offset") && ok;
    ok      = expectEqualUInt(mapGroupZOffset, 20, "z offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data      = MapGroupPacket::PacketData{};
    data.UniqueID  = 0x11223344;
    data.zone      = -1234;
    data.padding0A = 0;
    data.x         = 1.5F;
    data.y         = -2.25F;
    data.z         = 3.75F;

    return expectStructBytes(data, std::array<uint8, 20>{
                                       0x44, 0x33, 0x22, 0x11,
                                       0x2E, 0xFB, 0x00, 0x00,
                                       0x00, 0x00, 0xC0, 0x3F,
                                       0x00, 0x00, 0x10, 0xC0,
                                       0x00, 0x00, 0x70, 0x40,
                                   },
                             "MAP_GROUP PacketData bytes");
}

auto testRuntimePlan() -> bool
{
    const auto packet = mapgroupserverhelpers::PlanFor({
        .uniqueId = 0x11223344,
        .zone     = -1234,
        .x        = 1.5F,
        .y        = -2.25F,
        .z        = 3.75F,
    });

    bool ok = true;
    ok      = expectEqualUInt(packet.UniqueID, 0x11223344, "runtime UniqueID") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(packet.zone), static_cast<uint16>(-1234), "runtime zone") && ok;
    ok      = expectEqualUInt(packet.padding0A, 0, "runtime padding") && ok;
    ok      = expectStructBytes(packet, std::array<uint8, 20>{
                                          0x44, 0x33, 0x22, 0x11,
                                          0x2E, 0xFB, 0x00, 0x00,
                                          0x00, 0x00, 0xC0, 0x3F,
                                          0x00, 0x00, 0x10, 0xC0,
                                          0x00, 0x00, 0x70, 0x40,
                                      },
                             "runtime packet bytes") && ok;
    return ok;
}

} // namespace

auto runS2CMapGroupPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testRuntimePlan() && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("s2c-map-group-packet", runS2CMapGroupPacketSelfTests);
