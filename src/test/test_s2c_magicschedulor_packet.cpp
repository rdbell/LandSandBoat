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

#include "test_s2c_magicschedulor_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/base_entity.h"
#include "map/packets/s2c/0x03a_magicschedulor.h"

namespace
{

constexpr auto magicSchedulorUniqueNoCasOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData, UniqueNoCas);
constexpr auto magicSchedulorUniqueNoTarOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData, UniqueNoTar);
constexpr auto magicSchedulorActIndexCastOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData, ActIndexCast);
constexpr auto magicSchedulorActIndexTarOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData, ActIndexTar);
constexpr auto magicSchedulorFileNumOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData, fileNum);
constexpr auto magicSchedulorTypeOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData, type);
constexpr auto magicSchedulorPadding00Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData, padding00);
constexpr auto magicSchedulorPacketSize         = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MAGICSCHEDULOR packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MAGICSCHEDULOR packet self-test failed: " << label << " got";
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
            std::cerr << "s2c MAGICSCHEDULOR packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
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
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_MAGICSCHEDULOR::PacketData), 16, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(magicSchedulorPacketSize, 20, "packet size") && ok;
    ok      = expectEqualUInt(magicSchedulorUniqueNoCasOffset, 4, "UniqueNoCas offset") && ok;
    ok      = expectEqualUInt(magicSchedulorUniqueNoTarOffset, 8, "UniqueNoTar offset") && ok;
    ok      = expectEqualUInt(magicSchedulorActIndexCastOffset, 12, "ActIndexCast offset") && ok;
    ok      = expectEqualUInt(magicSchedulorActIndexTarOffset, 14, "ActIndexTar offset") && ok;
    ok      = expectEqualUInt(magicSchedulorFileNumOffset, 16, "fileNum offset") && ok;
    ok      = expectEqualUInt(magicSchedulorTypeOffset, 18, "type offset") && ok;
    ok      = expectEqualUInt(magicSchedulorPadding00Offset, 19, "padding00 offset") && ok;
    return ok;
}

auto testEntityConstructor() -> bool
{
    auto caster = makeEntity(0x11223344, 0x5566);
    auto target = makeEntity(0xAABBCCDD, 0xEEFF);
    auto packet = GP_SERV_COMMAND_MAGICSCHEDULOR(&caster, &target, 0x7788, GP_SERV_COMMAND_MAGICSCHEDULOR_TYPE::WeaponSkill);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x03A, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), magicSchedulorPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x3A, 0x0A, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, magicSchedulorUniqueNoCasOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNoCas") && ok;
    ok      = expectBytes(packet, magicSchedulorUniqueNoTarOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "UniqueNoTar") && ok;
    ok      = expectBytes(packet, magicSchedulorActIndexCastOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndexCast") && ok;
    ok      = expectBytes(packet, magicSchedulorActIndexTarOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "ActIndexTar") && ok;
    ok      = expectBytes(packet, magicSchedulorFileNumOffset, std::array<uint8, 2>{ 0x88, 0x77 }, "fileNum") && ok;
    ok      = expectBytes(packet, magicSchedulorTypeOffset, std::array<uint8, 1>{ 0x06 }, "type") && ok;
    ok      = expectBytes(packet, magicSchedulorPadding00Offset, std::array<uint8, 1>{ 0x00 }, "padding00") && ok;
    ok      = expectZeroRange(packet, magicSchedulorPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testNullEntityConstructorLeavesPayloadZeroed() -> bool
{
    auto packet = GP_SERV_COMMAND_MAGICSCHEDULOR(nullptr, nullptr, 0x7788, GP_SERV_COMMAND_MAGICSCHEDULOR_TYPE::MonsterSkill);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x03A, "null type") && ok;
    ok      = expectEqualUInt(packet.getSize(), magicSchedulorPacketSize, "null size") && ok;
    ok      = expectZeroRange(packet, magicSchedulorUniqueNoCasOffset, magicSchedulorPacketSize, "null payload") && ok;
    ok      = expectZeroRange(packet, magicSchedulorPacketSize, PACKET_SIZE, "null tail") && ok;
    return ok;
}

} // namespace

auto runS2CMagicSchedulorPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEntityConstructor() && ok;
    ok      = testNullEntityConstructorLeavesPayloadZeroed() && ok;
    return ok;
}
