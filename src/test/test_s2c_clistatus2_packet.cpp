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

#include "test_s2c_clistatus2_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "map/packets/s2c/0x062_clistatus2.h"

namespace
{

using Packet = GP_SERV_COMMAND_CLISTATUS2;

constexpr auto commandRecastOffset = sizeof(GP_SERV_HEADER) + offsetof(Packet::PacketData, CommandRecast);
constexpr auto skillBaseOffset     = sizeof(GP_SERV_HEADER) + offsetof(Packet::PacketData, skill_base);
constexpr auto commandRecastCount  = sizeof(Packet::PacketData::CommandRecast) / sizeof(uint32_t);
constexpr auto skillBaseCount      = sizeof(Packet::PacketData::skill_base) / sizeof(uint16_t);
constexpr auto packetDataSize      = sizeof(Packet::PacketData);
constexpr auto packetSize          = sizeof(GP_SERV_HEADER) + packetDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CLISTATUS2 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CLISTATUS2 packet self-test failed: " << label << " got";
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
            std::cerr << "s2c CLISTATUS2 packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectSkill(CBasicPacket& packet, std::size_t index, uint16_t expected, const std::string& label) -> bool
{
    const auto offset = skillBaseOffset + index * sizeof(uint16_t);
    return expectBytes(packet, offset, std::array<uint8, 2>{ static_cast<uint8>(expected), static_cast<uint8>(expected >> 8) }, label);
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_CLISTATUS2), 0x062, "CLISTATUS2 packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(commandRecastCount, 31, "CommandRecast count") && ok;
    ok      = expectEqualUInt(skillBaseCount, 64, "skill_base count") && ok;
    ok      = expectEqualUInt(packetDataSize, 252, "PacketData size") && ok;
    ok      = expectEqualUInt(packetSize, 256, "packet size") && ok;
    ok      = expectEqualUInt(commandRecastOffset, 4, "CommandRecast offset") && ok;
    ok      = expectEqualUInt(skillBaseOffset, 128, "skill_base offset") && ok;
    return ok;
}

auto testConstructorBytes() -> bool
{
    auto character = CCharEntity{};
    for (std::size_t i = 0; i < skillBaseCount; ++i)
    {
        character.WorkingSkills.skill[i] = static_cast<uint16>(0x1000 + i);
    }

    auto packet = Packet(&character);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x062, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), packetSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x62, 0x80, 0xEF, 0xBE }, "header") && ok;
    ok      = expectZeroRange(packet, commandRecastOffset, skillBaseOffset, "CommandRecast") && ok;
    ok      = expectSkill(packet, 0, 0x1000, "skill 0") && ok;
    ok      = expectSkill(packet, 21, 0x1015, "skill 21") && ok;
    ok      = expectSkill(packet, 22, 0x8000, "automaton melee override") && ok;
    ok      = expectSkill(packet, 23, 0x8000, "automaton ranged override") && ok;
    ok      = expectSkill(packet, 24, 0x8000, "automaton magic override") && ok;
    ok      = expectSkill(packet, 25, 0x1019, "skill 25") && ok;
    ok      = expectSkill(packet, 63, 0x103F, "skill 63") && ok;
    return ok;
}

} // namespace

auto runS2CCliStatus2PacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testConstructorBytes() && ok;
    return ok;
}
