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

#include "test_s2c_talknum_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/base_entity.h"
#include "map/packets/s2c/0x036_talknum.h"

namespace
{

constexpr auto talkNumUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUM::PacketData, UniqueNo);
constexpr auto talkNumActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUM::PacketData, ActIndex);
constexpr auto talkNumMesNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUM::PacketData, MesNum);
constexpr auto talkNumTypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUM::PacketData, Type);
constexpr auto talkNumPadding0DOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUM::PacketData, padding0D);
constexpr auto talkNumPadding0EOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUM::PacketData, padding0E);
constexpr auto talkNumPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_TALKNUM::PacketData);

struct EntityStorage
{
    alignas(CBaseEntity) std::array<std::byte, sizeof(CBaseEntity)> bytes{};

    auto entity() -> CBaseEntity*
    {
        return reinterpret_cast<CBaseEntity*>(bytes.data());
    }
};

template <typename T>
void writeEntityField(EntityStorage& storage, std::size_t offset, const T& value)
{
    std::memcpy(storage.bytes.data() + offset, &value, sizeof(value));
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif

auto makeEntity(std::uint32_t id, std::uint16_t targid, ENTITYTYPE objtype) -> EntityStorage
{
    auto storage = EntityStorage{};
    writeEntityField(storage, offsetof(CBaseEntity, id), id);
    writeEntityField(storage, offsetof(CBaseEntity, targid), targid);
    writeEntityField(storage, offsetof(CBaseEntity, objtype), objtype);
    return storage;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TALKNUM packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TALKNUM packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TALKNUM::PacketData), 12, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(talkNumPacketSize, 16, "packet size") && ok;
    ok      = expectEqualUInt(talkNumUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(talkNumActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(talkNumMesNumOffset, 10, "MesNum offset") && ok;
    ok      = expectEqualUInt(talkNumTypeOffset, 12, "Type offset") && ok;
    ok      = expectEqualUInt(talkNumPadding0DOffset, 13, "padding0D offset") && ok;
    ok      = expectEqualUInt(talkNumPadding0EOffset, 14, "padding0E offset") && ok;
    return ok;
}

auto testNPCShowNameConstructor() -> bool
{
    auto entity = makeEntity(0x11223344, 0x5566, ENTITYTYPE::TYPE_NPC);
    auto packet = GP_SERV_COMMAND_TALKNUM(entity.entity(), 0x1234);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x036, "npc type") && ok;
    ok      = expectEqualUInt(packet.getSize(), talkNumPacketSize, "npc size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x36, 0x08, 0xEF, 0xBE }, "npc header") && ok;
    ok      = expectBytes(packet, talkNumUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "npc UniqueNo") && ok;
    ok      = expectBytes(packet, talkNumActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "npc ActIndex") && ok;
    ok      = expectBytes(packet, talkNumMesNumOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "npc MesNum") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumTypeOffset], 0, "npc Type") && ok;
    ok      = expectZeroRange(packet, talkNumPadding0DOffset, talkNumPacketSize, "npc padding") && ok;
    ok      = expectZeroRange(packet, talkNumPacketSize, PACKET_SIZE, "npc tail") && ok;
    return ok;
}

auto testPCAlwaysHidesNameConstructor() -> bool
{
    auto entity = makeEntity(0x01020304, 0x0708, ENTITYTYPE::TYPE_PC);
    auto packet = GP_SERV_COMMAND_TALKNUM(entity.entity(), 0x0142, true, 7);

    bool ok = true;
    ok      = expectBytes(packet, talkNumUniqueNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "pc UniqueNo") && ok;
    ok      = expectBytes(packet, talkNumActIndexOffset, std::array<uint8, 2>{ 0x08, 0x07 }, "pc ActIndex") && ok;
    ok      = expectBytes(packet, talkNumMesNumOffset, std::array<uint8, 2>{ 0x42, 0x81 }, "pc MesNum") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumTypeOffset], 7, "pc Type") && ok;
    ok      = expectZeroRange(packet, talkNumPadding0DOffset, talkNumPacketSize, "pc padding") && ok;
    return ok;
}

auto testShowNameFalseSetsMessageHighBit() -> bool
{
    auto entity = makeEntity(0xAABBCCDD, 0xEEFF, ENTITYTYPE::TYPE_NPC);
    auto packet = GP_SERV_COMMAND_TALKNUM(entity.entity(), 0x1234, false, 3);

    bool ok = true;
    ok      = expectBytes(packet, talkNumMesNumOffset, std::array<uint8, 2>{ 0x34, 0x92 }, "hidden MesNum") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumTypeOffset], 3, "hidden Type") && ok;
    ok      = expectZeroRange(packet, talkNumPadding0DOffset, talkNumPacketSize, "hidden padding") && ok;
    return ok;
}

auto testHighMessageIDWrapsWhenNameHidden() -> bool
{
    auto entity = makeEntity(0x99AABBCC, 0x1122, ENTITYTYPE::TYPE_PC);
    auto packet = GP_SERV_COMMAND_TALKNUM(entity.entity(), 0x9001);

    bool ok = true;
    ok      = expectBytes(packet, talkNumMesNumOffset, std::array<uint8, 2>{ 0x01, 0x10 }, "wrapped MesNum") && ok;
    ok      = expectZeroRange(packet, talkNumPadding0DOffset, talkNumPacketSize, "wrapped padding") && ok;
    return ok;
}

} // namespace

auto runS2CTalkNumPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testNPCShowNameConstructor() && ok;
    ok      = testPCAlwaysHidesNameConstructor() && ok;
    ok      = testShowNameFalseSetsMessageHighBit() && ok;
    ok      = testHighMessageIDWrapsWhenNameHidden() && ok;
    return ok;
}
