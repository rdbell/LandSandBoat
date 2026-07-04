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

#include "test_s2c_talknumwork_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

#include "entities/char_entity.h"
#include "map/entities/base_entity.h"
#include "map/packets/s2c/0x02a_talknumwork.h"

namespace
{

constexpr auto talkNumWorkUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, UniqueNo);
constexpr auto talkNumWorkNumOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, num);
constexpr auto talkNumWorkActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, ActIndex);
constexpr auto talkNumWorkMesNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, MesNum);
constexpr auto talkNumWorkTypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, Type);
constexpr auto talkNumWorkFlagOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, Flag);
constexpr auto talkNumWorkStringOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, String);
constexpr auto talkNumWorkPadding3EOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TALKNUMWORK::PacketData, padding3E);
constexpr auto talkNumWorkPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_TALKNUMWORK::PacketData);
constexpr auto talkNumWorkNumElementSize  = sizeof(GP_SERV_COMMAND_TALKNUMWORK::PacketData::num[0]);
constexpr auto talkNumWorkStringLen       = sizeof(GP_SERV_COMMAND_TALKNUMWORK::PacketData::String);
constexpr auto talkNumWorkPadding3ELen    = sizeof(GP_SERV_COMMAND_TALKNUMWORK::PacketData::padding3E);

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

auto makeStoredEntity(std::uint32_t id, std::uint16_t targid, ENTITYTYPE objtype) -> EntityStorage
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

auto makeChar(std::uint32_t id, std::uint16_t targid, std::string name) -> CCharEntity
{
    auto character  = CCharEntity{};
    character.id    = id;
    character.targid = targid;
    character.name   = std::move(name);
    return character;
}

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUMWORK packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TALKNUMWORK packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TALKNUMWORK packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TALKNUMWORK packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto stringBytes(std::string_view name) -> std::array<uint8, talkNumWorkStringLen>
{
    auto bytes = std::array<uint8, talkNumWorkStringLen>{};
    std::memcpy(bytes.data(), name.data(), std::min<std::size_t>(name.size(), bytes.size() - 1));
    return bytes;
}

auto numOffset(std::size_t index) -> std::size_t
{
    return talkNumWorkNumOffset + index * talkNumWorkNumElementSize;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(talkNumWorkNumElementSize, 4, "num element size") && ok;
    ok      = expectEqualUInt(talkNumWorkStringLen, 32, "String size") && ok;
    ok      = expectEqualUInt(talkNumWorkPadding3ELen, 2, "padding3E size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_TALKNUMWORK::PacketData), 60, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(talkNumWorkPacketSize, 64, "packet size") && ok;
    ok      = expectEqualUInt(talkNumWorkUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(talkNumWorkNumOffset, 8, "num offset") && ok;
    ok      = expectEqualUInt(talkNumWorkActIndexOffset, 24, "ActIndex offset") && ok;
    ok      = expectEqualUInt(talkNumWorkMesNumOffset, 26, "MesNum offset") && ok;
    ok      = expectEqualUInt(talkNumWorkTypeOffset, 28, "Type offset") && ok;
    ok      = expectEqualUInt(talkNumWorkFlagOffset, 29, "Flag offset") && ok;
    ok      = expectEqualUInt(talkNumWorkStringOffset, 30, "String offset") && ok;
    ok      = expectEqualUInt(talkNumWorkPadding3EOffset, 62, "padding3E offset") && ok;
    return ok;
}

auto testNPCNoNameConstructor() -> bool
{
    auto entity = makeStoredEntity(0x11223344, 0x5566, ENTITYTYPE::TYPE_NPC);
    auto packet = GP_SERV_COMMAND_TALKNUMWORK(entity.entity(), 0x1234, 0x01020304, 0xA0B0C0D0, 3, 0xFFFFFFFF);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x02A, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), talkNumWorkPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x2A, 0x20, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, talkNumWorkUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNo") && ok;
    ok      = expectBytes(packet, numOffset(0), std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "num[0]") && ok;
    ok      = expectBytes(packet, numOffset(1), std::array<uint8, 4>{ 0xD0, 0xC0, 0xB0, 0xA0 }, "num[1]") && ok;
    ok      = expectBytes(packet, numOffset(3), std::array<uint8, 4>{ 0xFF, 0xFF, 0xFF, 0xFF }, "num[3]") && ok;
    ok      = expectBytes(packet, talkNumWorkActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndex") && ok;
    ok      = expectBytes(packet, talkNumWorkMesNumOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "MesNum") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumWorkTypeOffset], 0, "Type") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumWorkFlagOffset], 0, "Flag") && ok;
    ok      = expectZeroRange(packet, talkNumWorkStringOffset, talkNumWorkPacketSize, "String and padding") && ok;
    ok      = expectZeroRange(packet, talkNumWorkPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testPCHidesNameWhenShowNameFalse() -> bool
{
    auto character = makeChar(0x01020304, 0x0708, "Alice");
    auto packet    = GP_SERV_COMMAND_TALKNUMWORK(&character, 0x0142);

    bool ok = true;
    ok      = expectBytes(packet, talkNumWorkUniqueNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "pc UniqueNo") && ok;
    ok      = expectBytes(packet, talkNumWorkActIndexOffset, std::array<uint8, 2>{ 0x08, 0x07 }, "pc ActIndex") && ok;
    ok      = expectBytes(packet, talkNumWorkMesNumOffset, std::array<uint8, 2>{ 0x42, 0x81 }, "pc MesNum") && ok;
    ok      = expectZeroRange(packet, talkNumWorkStringOffset, talkNumWorkPacketSize, "pc String and padding") && ok;
    return ok;
}

auto testShowNameCopiesAndTruncatesRawName() -> bool
{
    auto character = makeChar(0xAABBCCDD, 0xEEFF, std::string("ab\0cdefghijklmnopqrstuvwxyz012345", 33));
    auto packet    = GP_SERV_COMMAND_TALKNUMWORK(&character, 0x9001, 1, 2, 3, 4, true);

    bool ok = true;
    ok      = expectBytes(packet, talkNumWorkMesNumOffset, std::array<uint8, 2>{ 0x01, 0x90 }, "show-name MesNum") && ok;
    ok      = expectBytes(packet, talkNumWorkStringOffset, stringBytes(std::string_view("ab\0cdefghijklmnopqrstuvwxyz0123", 31)), "show-name String") && ok;
    ok      = expectEqualInt(packetData(packet)[talkNumWorkStringOffset + 31], 0, "show-name String[31]") && ok;
    ok      = expectZeroRange(packet, talkNumWorkPadding3EOffset, talkNumWorkPacketSize, "show-name padding") && ok;
    return ok;
}

} // namespace

auto runS2CTalkNumWorkPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testNPCNoNameConstructor() && ok;
    ok      = testPCHidesNameWhenShowNameFalse() && ok;
    ok      = testShowNameCopiesAndTruncatesRawName() && ok;
    return ok;
}
