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

#include "test_s2c_mapschedulor_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/base_entity.h"
#include "map/packets/s2c/0x039_mapschedulor.h"

namespace
{

constexpr auto mapSchedulorUniqueNoCasOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAPSCHEDULOR::PacketData, UniqueNoCas);
constexpr auto mapSchedulorUniqueNoTarOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAPSCHEDULOR::PacketData, UniqueNoTar);
constexpr auto mapSchedulorIDOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAPSCHEDULOR::PacketData, id);
constexpr auto mapSchedulorActIndexCastOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAPSCHEDULOR::PacketData, ActIndexCast);
constexpr auto mapSchedulorActIndexTarOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAPSCHEDULOR::PacketData, ActIndexTar);
constexpr auto mapSchedulorPacketSize         = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_MAPSCHEDULOR::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MAPSCHEDULOR packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MAPSCHEDULOR packet self-test failed: " << label << " got";
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
            std::cerr << "s2c MAPSCHEDULOR packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto makeEntity(std::uint32_t id, std::uint16_t targid) -> CBaseEntity
{
    auto entity  = CBaseEntity{};
    entity.id    = id;
    entity.targid = targid;
    return entity;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_MAPSCHEDULOR::PacketData), 16, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(mapSchedulorPacketSize, 20, "packet size") && ok;
    ok      = expectEqualUInt(mapSchedulorUniqueNoCasOffset, 4, "UniqueNoCas offset") && ok;
    ok      = expectEqualUInt(mapSchedulorUniqueNoTarOffset, 8, "UniqueNoTar offset") && ok;
    ok      = expectEqualUInt(mapSchedulorIDOffset, 12, "id offset") && ok;
    ok      = expectEqualUInt(mapSchedulorActIndexCastOffset, 16, "ActIndexCast offset") && ok;
    ok      = expectEqualUInt(mapSchedulorActIndexTarOffset, 18, "ActIndexTar offset") && ok;
    return ok;
}

auto testEntityConstructor() -> bool
{
    auto entity = makeEntity(0x11223344, 0x5566);
    auto packet = GP_SERV_COMMAND_MAPSCHEDULOR(&entity, "door");
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x039, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), mapSchedulorPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x39, 0x0A, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, mapSchedulorUniqueNoCasOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNoCas") && ok;
    ok      = expectBytes(packet, mapSchedulorUniqueNoTarOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNoTar") && ok;
    ok      = expectBytes(packet, mapSchedulorIDOffset, std::array<uint8, 4>{ 'd', 'o', 'o', 'r' }, "id") && ok;
    ok      = expectBytes(packet, mapSchedulorActIndexCastOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndexCast") && ok;
    ok      = expectBytes(packet, mapSchedulorActIndexTarOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndexTar") && ok;
    ok      = expectZeroRange(packet, mapSchedulorPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testNullEntityConstructorOnlyWritesType() -> bool
{
    auto packet = GP_SERV_COMMAND_MAPSCHEDULOR(nullptr, "null");

    bool ok = true;
    ok      = expectZeroRange(packet, mapSchedulorUniqueNoCasOffset, mapSchedulorIDOffset, "null entity ids") && ok;
    ok      = expectBytes(packet, mapSchedulorIDOffset, std::array<uint8, 4>{ 'n', 'u', 'l', 'l' }, "null id") && ok;
    ok      = expectZeroRange(packet, mapSchedulorActIndexCastOffset, mapSchedulorPacketSize, "null indexes") && ok;
    ok      = expectZeroRange(packet, mapSchedulorPacketSize, PACKET_SIZE, "null tail") && ok;
    return ok;
}

auto testRawFourByteTagCopiesNuls() -> bool
{
    const char tag[4] = { 'a', '\0', 'b', '\0' };
    auto       packet = GP_SERV_COMMAND_MAPSCHEDULOR(nullptr, tag);

    bool ok = true;
    ok      = expectBytes(packet, mapSchedulorIDOffset, std::array<uint8, 4>{ 'a', 0, 'b', 0 }, "raw id") && ok;
    ok      = expectZeroRange(packet, mapSchedulorUniqueNoCasOffset, mapSchedulorIDOffset, "raw ids") && ok;
    return ok;
}

} // namespace

auto runS2CMapSchedulorPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEntityConstructor() && ok;
    ok      = testNullEntityConstructorOnlyWritesType() && ok;
    ok      = testRawFourByteTagCopiesNuls() && ok;
    return ok;
}
