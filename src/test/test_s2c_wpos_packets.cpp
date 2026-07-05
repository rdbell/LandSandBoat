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

#include "test_s2c_wpos_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/mmo.h"
#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x05b_wpos.h"
#include "map/packets/s2c/0x065_wpos2.h"

namespace
{

constexpr auto wposXOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, x);
constexpr auto wposYOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, y);
constexpr auto wposZOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, z);
constexpr auto wposUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, UniqueNo);
constexpr auto wposActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, ActIndex);
constexpr auto wposModeOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, Mode);
constexpr auto wposDirOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, dir);
constexpr auto wposPaddingOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS::PacketData, padding18);
constexpr auto wposPacketSize     = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_WPOS::PacketData);

constexpr auto wpos2XOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, x);
constexpr auto wpos2YOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, y);
constexpr auto wpos2ZOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, z);
constexpr auto wpos2UniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, UniqueNo);
constexpr auto wpos2ActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, ActIndex);
constexpr auto wpos2ModeOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, Mode);
constexpr auto wpos2DirOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, dir);
constexpr auto wpos2PaddingOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WPOS2::PacketData, padding18);
constexpr auto wpos2PacketSize     = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_WPOS2::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void makeChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid, position_t position)
{
    character.id     = id;
    character.targid = targid;
    character.loc.p  = position;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c WPOS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualFloat(float actual, float expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c WPOS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c WPOS packet self-test failed: " << label << " got";
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
            std::cerr << "s2c WPOS packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectPosition(position_t actual, position_t expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectEqualFloat(actual.x, expected.x, label + " x") && ok;
    ok      = expectEqualFloat(actual.y, expected.y, label + " y") && ok;
    ok      = expectEqualFloat(actual.z, expected.z, label + " z") && ok;
    ok      = expectEqualUInt(actual.rotation, expected.rotation, label + " rotation") && ok;
    return ok;
}

auto expectWPosPayload(CBasicPacket& packet, std::size_t xOffset, std::size_t yOffset, std::size_t zOffset, std::size_t uniqueNoOffset, std::size_t actIndexOffset, std::size_t modeOffset, std::size_t dirOffset, POSMODE mode, std::uint8_t rotation) -> bool
{
    bool ok = true;
    ok      = expectBytes(packet, xOffset, std::array<uint8, 4>{ 0x00, 0x00, 0xC0, 0x3F }, "x") && ok;
    ok      = expectBytes(packet, yOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x10, 0xC0 }, "y") && ok;
    ok      = expectBytes(packet, zOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x70, 0x40 }, "z") && ok;
    ok      = expectBytes(packet, uniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNo") && ok;
    ok      = expectBytes(packet, actIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndex") && ok;
    ok      = expectEqualUInt(packetData(packet)[modeOffset], static_cast<std::uint8_t>(mode), "Mode") && ok;
    ok      = expectEqualUInt(packetData(packet)[dirOffset], rotation, "dir") && ok;
    return ok;
}

auto testWPosLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_WPOS::PacketData), 24, "sizeof(WPOS PacketData)") && ok;
    ok      = expectEqualUInt(wposPacketSize, 28, "WPOS packet size") && ok;
    ok      = expectEqualUInt(wposXOffset, 4, "WPOS x offset") && ok;
    ok      = expectEqualUInt(wposYOffset, 8, "WPOS y offset") && ok;
    ok      = expectEqualUInt(wposZOffset, 12, "WPOS z offset") && ok;
    ok      = expectEqualUInt(wposUniqueNoOffset, 16, "WPOS UniqueNo offset") && ok;
    ok      = expectEqualUInt(wposActIndexOffset, 20, "WPOS ActIndex offset") && ok;
    ok      = expectEqualUInt(wposModeOffset, 22, "WPOS Mode offset") && ok;
    ok      = expectEqualUInt(wposDirOffset, 23, "WPOS dir offset") && ok;
    ok      = expectEqualUInt(wposPaddingOffset, 24, "WPOS padding offset") && ok;
    return ok;
}

auto testWPosNormalConstructor() -> bool
{
    const auto oldPosition = position_t{ -8.0f, 9.0f, -10.0f, 0, 0x11 };
    const auto newPosition = position_t{ 1.5f, -2.25f, 3.75f, 0, 0x7F };
    auto       character   = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, oldPosition);
    auto packet = GP_SERV_COMMAND_WPOS(&character, newPosition, POSMODE::NORMAL);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x05B, "WPOS type") && ok;
    ok      = expectEqualUInt(packet.getSize(), wposPacketSize, "WPOS size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x5B, 0x0E, 0xEF, 0xBE }, "WPOS header") && ok;
    ok      = expectWPosPayload(packet, wposXOffset, wposYOffset, wposZOffset, wposUniqueNoOffset, wposActIndexOffset, wposModeOffset, wposDirOffset, POSMODE::NORMAL, newPosition.rotation) && ok;
    ok      = expectZeroRange(packet, wposPaddingOffset, wposPacketSize, "WPOS padding") && ok;
    ok      = expectZeroRange(packet, wposPacketSize, PACKET_SIZE, "WPOS tail") && ok;
    ok      = expectPosition(character.loc.p, newPosition, "character updated position") && ok;
    return ok;
}

auto testWPosRotateConstructor() -> bool
{
    const auto oldPosition = position_t{ 1.5f, -2.25f, 3.75f, 0, 0x11 };
    const auto newPosition = position_t{ -8.0f, 9.0f, -10.0f, 0, 0x7F };
    auto       character   = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, oldPosition);
    auto packet = GP_SERV_COMMAND_WPOS(&character, newPosition, POSMODE::ROTATE);
    auto       expected    = oldPosition;
    expected.rotation      = newPosition.rotation;

    bool ok = true;
    ok      = expectWPosPayload(packet, wposXOffset, wposYOffset, wposZOffset, wposUniqueNoOffset, wposActIndexOffset, wposModeOffset, wposDirOffset, POSMODE::ROTATE, newPosition.rotation) && ok;
    ok      = expectPosition(character.loc.p, expected, "character rotate position") && ok;
    return ok;
}

auto testWPosClearDoesNotMove() -> bool
{
    const auto oldPosition = position_t{ 1.5f, -2.25f, 3.75f, 0, 0x11 };
    const auto newPosition = position_t{ -8.0f, 9.0f, -10.0f, 0, 0x7F };
    auto       character   = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, oldPosition);
    auto packet = GP_SERV_COMMAND_WPOS(&character, newPosition, POSMODE::CLEAR);

    bool ok = true;
    ok      = expectWPosPayload(packet, wposXOffset, wposYOffset, wposZOffset, wposUniqueNoOffset, wposActIndexOffset, wposModeOffset, wposDirOffset, POSMODE::CLEAR, oldPosition.rotation) && ok;
    ok      = expectPosition(character.loc.p, oldPosition, "character clear position") && ok;
    return ok;
}

auto testWPos2LayoutAndConstructor() -> bool
{
    const auto oldPosition = position_t{ -8.0f, 9.0f, -10.0f, 0, 0x11 };
    const auto newPosition = position_t{ 1.5f, -2.25f, 3.75f, 0, 0x7F };
    auto       character   = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, oldPosition);
    auto packet = GP_SERV_COMMAND_WPOS2(&character, newPosition, POSMODE::MATERIALIZE);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_WPOS2::PacketData), 24, "sizeof(WPOS2 PacketData)") && ok;
    ok      = expectEqualUInt(wpos2PacketSize, 28, "WPOS2 packet size") && ok;
    ok      = expectEqualUInt(wpos2XOffset, 4, "WPOS2 x offset") && ok;
    ok      = expectEqualUInt(wpos2YOffset, 8, "WPOS2 y offset") && ok;
    ok      = expectEqualUInt(wpos2ZOffset, 12, "WPOS2 z offset") && ok;
    ok      = expectEqualUInt(wpos2UniqueNoOffset, 16, "WPOS2 UniqueNo offset") && ok;
    ok      = expectEqualUInt(wpos2ActIndexOffset, 20, "WPOS2 ActIndex offset") && ok;
    ok      = expectEqualUInt(wpos2ModeOffset, 22, "WPOS2 Mode offset") && ok;
    ok      = expectEqualUInt(wpos2DirOffset, 23, "WPOS2 dir offset") && ok;
    ok      = expectEqualUInt(wpos2PaddingOffset, 24, "WPOS2 padding offset") && ok;
    ok      = expectEqualUInt(packet.getType(), 0x065, "WPOS2 type") && ok;
    ok      = expectEqualUInt(packet.getSize(), wpos2PacketSize, "WPOS2 size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x65, 0x0E, 0xEF, 0xBE }, "WPOS2 header") && ok;
    ok      = expectWPosPayload(packet, wpos2XOffset, wpos2YOffset, wpos2ZOffset, wpos2UniqueNoOffset, wpos2ActIndexOffset, wpos2ModeOffset, wpos2DirOffset, POSMODE::MATERIALIZE, newPosition.rotation) && ok;
    ok      = expectZeroRange(packet, wpos2PaddingOffset, wpos2PacketSize, "WPOS2 padding") && ok;
    ok      = expectZeroRange(packet, wpos2PacketSize, PACKET_SIZE, "WPOS2 tail") && ok;
    ok      = expectPosition(character.loc.p, newPosition, "character updated WPOS2 position") && ok;
    return ok;
}

} // namespace

auto runS2CWPosPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testWPosLayout() && ok;
    ok      = testWPosNormalConstructor() && ok;
    ok      = testWPosRotateConstructor() && ok;
    ok      = testWPosClearDoesNotMove() && ok;
    ok      = testWPos2LayoutAndConstructor() && ok;
    return ok;
}
