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

#include "test_s2c_effect_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "map/enums/synthesis_effect.h"
#include "map/packets/s2c/0x030_effect.h"

namespace
{

constexpr auto effectUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EFFECT::PacketData, UniqueNo);
constexpr auto effectActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EFFECT::PacketData, ActIndex);
constexpr auto effectEffectNumOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EFFECT::PacketData, EffectNum);
constexpr auto effectTypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EFFECT::PacketData, Type);
constexpr auto effectStatusOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EFFECT::PacketData, Status);
constexpr auto effectTimerOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EFFECT::PacketData, Timer);
constexpr auto effectPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_EFFECT::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto makeChar(std::uint32_t id, std::uint16_t targid, std::uint8_t animation) -> CCharEntity
{
    auto character      = CCharEntity{};
    character.id        = id;
    character.targid    = targid;
    character.animation = animation;
    return character;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EFFECT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EFFECT packet self-test failed: " << label << " got";
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
            std::cerr << "s2c EFFECT packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_EFFECT::PacketData), 12, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(effectPacketSize, 16, "packet size") && ok;
    ok      = expectEqualUInt(effectUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(effectActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(effectEffectNumOffset, 10, "EffectNum offset") && ok;
    ok      = expectEqualUInt(effectTypeOffset, 12, "Type offset") && ok;
    ok      = expectEqualUInt(effectStatusOffset, 13, "Status offset") && ok;
    ok      = expectEqualUInt(effectTimerOffset, 14, "Timer offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto character = makeChar(0x11223344, 0x5566, 0x22);
    auto packet    = GP_SERV_COMMAND_EFFECT(&character, SynthesisEffect::Dark, 0x7F);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x030, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), effectPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x30, 0x08, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, effectUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "UniqueNo") && ok;
    ok      = expectBytes(packet, effectActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "ActIndex") && ok;
    ok      = expectBytes(packet, effectEffectNumOffset, std::array<uint8, 2>{ 0x17, 0x00 }, "EffectNum") && ok;
    ok      = expectBytes(packet, effectTypeOffset, std::array<uint8, 1>{ 0x7F }, "Type") && ok;
    ok      = expectBytes(packet, effectStatusOffset, std::array<uint8, 1>{ 0x22 }, "Status") && ok;
    ok      = expectBytes(packet, effectTimerOffset, std::array<uint8, 2>{ 0x00, 0x00 }, "Timer") && ok;
    ok      = expectZeroRange(packet, effectPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

} // namespace

auto runS2CEffectPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
