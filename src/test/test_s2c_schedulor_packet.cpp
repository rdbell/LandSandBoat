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

#include "test_s2c_schedulor_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/base_entity.h"
#include "map/enums/four_cc.h"
#include "map/packets/s2c/0x038_schedulor.h"

namespace
{

constexpr auto schedulorUniqueNoCasOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SCHEDULOR::PacketData, UniqueNoCas);
constexpr auto schedulorUniqueNoTarOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SCHEDULOR::PacketData, UniqueNoTar);
constexpr auto schedulorIDOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SCHEDULOR::PacketData, id);
constexpr auto schedulorActIndexCastOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SCHEDULOR::PacketData, ActIndexCast);
constexpr auto schedulorActIndexTarOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SCHEDULOR::PacketData, ActIndexTar);
constexpr auto schedulorPacketSize         = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_SCHEDULOR::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c SCHEDULOR packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c SCHEDULOR packet self-test failed: " << label << " got";
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
            std::cerr << "s2c SCHEDULOR packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
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
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_SCHEDULOR::PacketData), 16, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(schedulorPacketSize, 20, "packet size") && ok;
    ok      = expectEqualUInt(schedulorUniqueNoCasOffset, 4, "UniqueNoCas offset") && ok;
    ok      = expectEqualUInt(schedulorUniqueNoTarOffset, 8, "UniqueNoTar offset") && ok;
    ok      = expectEqualUInt(schedulorIDOffset, 12, "id offset") && ok;
    ok      = expectEqualUInt(schedulorActIndexCastOffset, 16, "ActIndexCast offset") && ok;
    ok      = expectEqualUInt(schedulorActIndexTarOffset, 18, "ActIndexTar offset") && ok;
    return ok;
}

auto testFourCCConstructor() -> bool
{
    auto caster = makeEntity(0x11223344, 0x5566);
    auto target = makeEntity(0xAABBCCDD, 0xEEFF);
    auto packet = GP_SERV_COMMAND_SCHEDULOR(&caster, &target, FourCC::FadeOut);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x038, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), schedulorPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x38, 0x0A, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, schedulorUniqueNoCasOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNoCas") && ok;
    ok      = expectBytes(packet, schedulorUniqueNoTarOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "UniqueNoTar") && ok;
    ok      = expectBytes(packet, schedulorIDOffset, std::array<uint8, 4>{ 'k', 'e', 's', 'u' }, "id") && ok;
    ok      = expectBytes(packet, schedulorActIndexCastOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndexCast") && ok;
    ok      = expectBytes(packet, schedulorActIndexTarOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "ActIndexTar") && ok;
    ok      = expectZeroRange(packet, schedulorPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testRawTagConstructor() -> bool
{
    auto       caster = makeEntity(0x01020304, 0x0506);
    auto       target = makeEntity(0x0708090A, 0x0B0C);
    const char tag[4] = { 'a', '\0', 'b', '\0' };
    auto       packet = GP_SERV_COMMAND_SCHEDULOR(&caster, &target, tag);

    bool ok = true;
    ok      = expectBytes(packet, schedulorUniqueNoCasOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "raw UniqueNoCas") && ok;
    ok      = expectBytes(packet, schedulorUniqueNoTarOffset, std::array<uint8, 4>{ 0x0A, 0x09, 0x08, 0x07 }, "raw UniqueNoTar") && ok;
    ok      = expectBytes(packet, schedulorIDOffset, std::array<uint8, 4>{ 'a', 0, 'b', 0 }, "raw id") && ok;
    ok      = expectBytes(packet, schedulorActIndexCastOffset, std::array<uint8, 2>{ 0x06, 0x05 }, "raw ActIndexCast") && ok;
    ok      = expectBytes(packet, schedulorActIndexTarOffset, std::array<uint8, 2>{ 0x0C, 0x0B }, "raw ActIndexTar") && ok;
    return ok;
}

} // namespace

auto runS2CSchedulorPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testFourCCConstructor() && ok;
    ok      = testRawTagConstructor() && ok;
    return ok;
}
