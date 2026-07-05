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

#include "test_s2c_assist_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/battle_entity.h"
#include "entities/char_entity.h"
#include "map/packets/s2c/0x058_assist.h"

namespace
{

constexpr auto assistUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ASSIST::PacketData, UniqueNo);
constexpr auto assistAssistNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ASSIST::PacketData, AssistNo);
constexpr auto assistActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ASSIST::PacketData, ActIndex);
constexpr auto assistPaddingOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ASSIST::PacketData, padding00);
constexpr auto assistPacketSize     = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_ASSIST::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void populateChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid)
{
    character.id     = id;
    character.targid = targid;
}

void populateTarget(CBattleEntity& target, std::uint32_t id)
{
    target.id = id;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ASSIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ASSIST packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ASSIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_ASSIST::PacketData), 12, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(assistPacketSize, 16, "packet size") && ok;
    ok      = expectEqualUInt(assistUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(assistAssistNoOffset, 8, "AssistNo offset") && ok;
    ok      = expectEqualUInt(assistActIndexOffset, 12, "ActIndex offset") && ok;
    ok      = expectEqualUInt(assistPaddingOffset, 14, "padding00 offset") && ok;
    return ok;
}

auto testConstructorWithTarget() -> bool
{
    auto character = CCharEntity{};
    auto target    = CBattleEntity{};
    populateChar(character, 0x11223344, 0x5566);
    populateTarget(target, 0xAABBCCDD);
    auto packet = GP_SERV_COMMAND_ASSIST(&character, &target);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x058, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), assistPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x58, 0x08, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, assistUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNo") && ok;
    ok      = expectBytes(packet, assistAssistNoOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "AssistNo") && ok;
    ok      = expectBytes(packet, assistActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndex") && ok;
    ok      = expectBytes(packet, assistPaddingOffset, std::array<uint8, 2>{ 0x00, 0x00 }, "padding00") && ok;
    ok      = expectZeroRange(packet, assistPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testConstructorWithoutTarget() -> bool
{
    auto character = CCharEntity{};
    populateChar(character, 0x11223344, 0x5566);
    auto packet = GP_SERV_COMMAND_ASSIST(&character, nullptr);

    bool ok = true;
    ok      = expectBytes(packet, assistUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNo") && ok;
    ok      = expectBytes(packet, assistAssistNoOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "nil AssistNo") && ok;
    ok      = expectBytes(packet, assistActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndex") && ok;
    return ok;
}

} // namespace

auto runS2CAssistPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructorWithTarget() && ok;
    ok      = testConstructorWithoutTarget() && ok;
    return ok;
}
