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

#include "test_s2c_entity_vis_packet.h"

#include "test/omega_self_test_registry.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "map/packets/s2c/0x077_entity_vis.h"

namespace
{

constexpr auto entityVisFlagsOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ENTITY_VIS::PacketData, Flags);
constexpr auto entityVisPaddingOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ENTITY_VIS::PacketData, padding05);
constexpr auto entityVisUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ENTITY_VIS::PacketData, UniqueNo);
constexpr auto entityVisPacketSize     = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_ENTITY_VIS::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ENTITY_VIS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ENTITY_VIS packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ENTITY_VIS packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_ENTITY_VIS::PacketData), 132, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(entityVisPacketSize, 136, "packet size") && ok;
    ok      = expectEqualUInt(entityVisFlagsOffset, 4, "Flags offset") && ok;
    ok      = expectEqualUInt(entityVisPaddingOffset, 5, "padding05 offset") && ok;
    ok      = expectEqualUInt(entityVisUniqueNoOffset, 8, "UniqueNo offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto ids    = std::vector<uint32>{ 0x11223344, 0xAABBCCDD, 0x01020304 };
    auto packet = GP_SERV_COMMAND_ENTITY_VIS(ids);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x077, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), entityVisPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x77, 0x44, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, entityVisFlagsOffset, std::array<uint8, 1>{ 0x01 }, "Flags") && ok;
    ok      = expectBytes(packet, entityVisPaddingOffset, std::array<uint8, 3>{ 0x00, 0x00, 0x00 }, "padding05") && ok;
    ok      = expectBytes(packet, entityVisUniqueNoOffset, std::array<uint8, 12>{ 0x44, 0x33, 0x22, 0x11, 0xDD, 0xCC, 0xBB, 0xAA, 0x04, 0x03, 0x02, 0x01 }, "UniqueNo values") && ok;
    ok      = expectZeroRange(packet, entityVisUniqueNoOffset + 12, entityVisPacketSize, "remaining UniqueNo values") && ok;
    ok      = expectZeroRange(packet, entityVisPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testMaximumEntityList() -> bool
{
    auto ids = std::vector<uint32>{};
    ids.reserve(32);
    for (std::uint32_t i = 0; i < 32; ++i)
    {
        ids.push_back(0x10000000 + i);
    }

    auto packet = GP_SERV_COMMAND_ENTITY_VIS(ids);
    bool ok     = true;
    for (std::size_t i = 0; i < ids.size(); ++i)
    {
        for (std::size_t byte = 0; byte < sizeof(uint32); ++byte)
        {
            ok = expectEqualUInt(packetData(packet)[entityVisUniqueNoOffset + i * sizeof(uint32) + byte],
                                 (ids[i] >> (byte * 8)) & 0xFF, "maximum list byte") && ok;
        }
    }
    ok = expectZeroRange(packet, entityVisPacketSize, PACKET_SIZE, "maximum list tail") && ok;
    return ok;
}

} // namespace

auto runS2CEntityVisPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    ok      = testMaximumEntityList() && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("s2c-entity-vis-packet", runS2CEntityVisPacketSelfTests);
