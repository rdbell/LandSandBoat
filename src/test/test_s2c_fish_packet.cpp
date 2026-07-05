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

#include "test_s2c_fish_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x115_fish.h"

namespace
{

constexpr auto fishStaminaOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, stamina);
constexpr auto fishArrowDelayOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, arrow_delay);
constexpr auto fishRegenOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, regen);
constexpr auto fishMoveFrequencyOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, move_frequency);
constexpr auto fishArrowDamageOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, arrow_damage);
constexpr auto fishArrowRegenOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, arrow_regen);
constexpr auto fishTimeOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, time);
constexpr auto fishAnglerSenseOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, angler_sense);
constexpr auto fishPadding13Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, padding13);
constexpr auto fishIntuitionOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FISH::PacketData, intuition);
constexpr auto fishPacketDataSize      = sizeof(GP_SERV_COMMAND_FISH::PacketData);
constexpr auto fishFullPacketSize      = sizeof(GP_SERV_HEADER) + fishPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c FISH packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c FISH packet self-test failed: " << label << " got";
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

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c FISH packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(fishPacketDataSize, 20, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(fishFullPacketSize, 24, "full packet size") && ok;
    ok      = expectEqualUInt(fishStaminaOffset, 4, "stamina offset") && ok;
    ok      = expectEqualUInt(fishArrowDelayOffset, 6, "arrow_delay offset") && ok;
    ok      = expectEqualUInt(fishRegenOffset, 8, "regen offset") && ok;
    ok      = expectEqualUInt(fishMoveFrequencyOffset, 10, "move_frequency offset") && ok;
    ok      = expectEqualUInt(fishArrowDamageOffset, 12, "arrow_damage offset") && ok;
    ok      = expectEqualUInt(fishArrowRegenOffset, 14, "arrow_regen offset") && ok;
    ok      = expectEqualUInt(fishTimeOffset, 16, "time offset") && ok;
    ok      = expectEqualUInt(fishAnglerSenseOffset, 18, "angler_sense offset") && ok;
    ok      = expectEqualUInt(fishPadding13Offset, 19, "padding13 offset") && ok;
    ok      = expectEqualUInt(fishIntuitionOffset, 20, "intuition offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_FISH(
        0x1122,
        0x3344,
        0x5566,
        0x7788,
        0x99AA,
        0xBBCC,
        0xDDEE,
        0x7F,
        0x01020304);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 24>{
        0x15, 0x0D, 0xEF, 0xBE,
        0x22, 0x11, 0xAA, 0x99,
        0x44, 0x33, 0x66, 0x55,
        0x88, 0x77, 0xCC, 0xBB,
        0xEE, 0xDD, 0x7F, 0x00,
        0x04, 0x03, 0x02, 0x01,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x115, "FISH type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "FISH size") && ok;
    ok      = expectBytes(packet, 0, expected, "encoded FISH packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "FISH tail") && ok;
    return ok;
}

} // namespace

auto runS2CFishPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
