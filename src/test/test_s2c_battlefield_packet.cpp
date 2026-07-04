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

#include "test_s2c_battlefield_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "map/packets/s2c/0x075_battlefield.h"

namespace
{

using BattlefieldPacketData = GP_SERV_COMMAND_BATTLEFIELD::PacketData;

constexpr auto battlefieldModeOffset        = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, Mode);
constexpr auto battlefieldTimestampOffset   = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, Timestamp);
constexpr auto battlefieldDurationOffset    = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, Duration);
constexpr auto battlefieldDurationWarnOffset = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, DurationWarn);
constexpr auto battlefieldFenceXOffset      = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, FenceX);
constexpr auto battlefieldFenceYOffset      = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, FenceY);
constexpr auto battlefieldFenceRadiusOffset = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, FenceRadius);
constexpr auto battlefieldFenceRotationOffset = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, FenceRotation);
constexpr auto battlefieldFlagsOffset       = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, Flags);
constexpr auto battlefieldFenceColorOffset  = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, FenceColor);
constexpr auto battlefieldUnknown26Offset   = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, unknown26);
constexpr auto battlefieldDataOffset        = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, Data);
constexpr auto battlefieldMesNumTitleOffset = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, MesNumTitle);
constexpr auto battlefieldMesNumDescriptionOffset = sizeof(GP_SERV_HEADER) + offsetof(BattlefieldPacketData, MesNumDescription);
constexpr auto battlefieldPacketSize        = sizeof(GP_SERV_HEADER) + sizeof(BattlefieldPacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto readU32(CBasicPacket& packet, std::size_t offset) -> std::uint32_t
{
    std::uint32_t value = 0;
    std::memcpy(&value, packetData(packet) + offset, sizeof(value));
    return value;
}

auto readI32(CBasicPacket& packet, std::size_t offset) -> std::int32_t
{
    std::int32_t value = 0;
    std::memcpy(&value, packetData(packet) + offset, sizeof(value));
    return value;
}

auto readU16(CBasicPacket& packet, std::size_t offset) -> std::uint16_t
{
    std::uint16_t value = 0;
    std::memcpy(&value, packetData(packet) + offset, sizeof(value));
    return value;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BATTLEFIELD packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BATTLEFIELD packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c BATTLEFIELD packet self-test failed: " << label << " got";
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
            std::cerr << "s2c BATTLEFIELD packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(scoreboard_data_t), 128, "sizeof(scoreboard_data_t)") && ok;
    ok      = expectEqualUInt(sizeof(progressbar_row_t), 20, "sizeof(progressbar_row_t)") && ok;
    ok      = expectEqualUInt(sizeof(progressbar_data_t), 124, "sizeof(progressbar_data_t)") && ok;
    ok      = expectEqualUInt(sizeof(BattlefieldPacketData), 168, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(battlefieldPacketSize, 172, "packet size") && ok;
    ok      = expectEqualUInt(battlefieldModeOffset, 4, "Mode offset") && ok;
    ok      = expectEqualUInt(battlefieldTimestampOffset, 8, "Timestamp offset") && ok;
    ok      = expectEqualUInt(battlefieldDurationOffset, 12, "Duration offset") && ok;
    ok      = expectEqualUInt(battlefieldDurationWarnOffset, 16, "DurationWarn offset") && ok;
    ok      = expectEqualUInt(battlefieldFenceXOffset, 20, "FenceX offset") && ok;
    ok      = expectEqualUInt(battlefieldFenceYOffset, 24, "FenceY offset") && ok;
    ok      = expectEqualUInt(battlefieldFenceRadiusOffset, 28, "FenceRadius offset") && ok;
    ok      = expectEqualUInt(battlefieldFenceRotationOffset, 32, "FenceRotation offset") && ok;
    ok      = expectEqualUInt(battlefieldFlagsOffset, 36, "Flags offset") && ok;
    ok      = expectEqualUInt(battlefieldFenceColorOffset, 37, "FenceColor offset") && ok;
    ok      = expectEqualUInt(battlefieldUnknown26Offset, 38, "unknown26 offset") && ok;
    ok      = expectEqualUInt(battlefieldDataOffset, 40, "Data offset") && ok;
    ok      = expectEqualUInt(battlefieldMesNumTitleOffset, 168, "MesNumTitle offset") && ok;
    ok      = expectEqualUInt(battlefieldMesNumDescriptionOffset, 170, "MesNumDescription offset") && ok;
    ok      = expectEqualUInt(OBJECTIVEUTILITY_COUNTDOWN, 0x01, "COUNTDOWN flag") && ok;
    ok      = expectEqualUInt(OBJECTIVEUTILITY_PROGRESS, 0x02, "PROGRESS flag") && ok;
    ok      = expectEqualUInt(OBJECTIVEUTILITY_HELP, 0x04, "HELP flag") && ok;
    ok      = expectEqualUInt(OBJECTIVEUTILITY_FENCE, 0x08, "FENCE flag") && ok;
    return ok;
}

auto testDefaultConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_BATTLEFIELD();
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x075, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), battlefieldPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x75, 0x56, 0xEF, 0xBE }, "header") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), battlefieldPacketSize, "payload") && ok;
    ok      = expectZeroRange(packet, battlefieldPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testCountdown() -> bool
{
    auto packet = GP_SERV_COMMAND_BATTLEFIELD(300, 45);

    bool ok = true;
    ok      = expectEqualUInt(readU32(packet, battlefieldModeOffset), 0x01, "countdown Mode") && ok;
    ok      = expectEqualUInt(packetData(packet)[battlefieldFlagsOffset], OBJECTIVEUTILITY_COUNTDOWN, "countdown Flags") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDurationOffset), 300, "Duration") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDurationWarnOffset), 45, "DurationWarn") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldTimestampOffset) == 0 ? 0 : 1, 1, "Timestamp nonzero") && ok;

    packet.addBars({ { "Lock", 9 } });
    packet.addCountdown(120, 30);
    ok = expectEqualUInt(readU32(packet, battlefieldModeOffset), 0xFFFF, "countdown preserves nonzero Mode") && ok;
    ok = expectEqualUInt(packetData(packet)[battlefieldFlagsOffset], OBJECTIVEUTILITY_COUNTDOWN | OBJECTIVEUTILITY_PROGRESS, "combined countdown/progress Flags") && ok;
    ok = expectEqualUInt(readU32(packet, battlefieldDurationOffset), 120, "updated Duration") && ok;
    ok = expectEqualUInt(readU32(packet, battlefieldDurationWarnOffset), 30, "updated DurationWarn") && ok;
    return ok;
}

auto testBars() -> bool
{
    auto packet = GP_SERV_COMMAND_BATTLEFIELD();
    packet.addBars({ { "AlphaObjectiveXY", 11 }, { "Beta", 22 }, { "", 33 } });

    bool ok = true;
    ok      = expectEqualUInt(readU32(packet, battlefieldModeOffset), 0xFFFF, "bars Mode") && ok;
    ok      = expectEqualUInt(packetData(packet)[battlefieldFlagsOffset], OBJECTIVEUTILITY_PROGRESS, "bars Flags") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset), 11, "bar0 Progress") && ok;
    ok      = expectBytes(packet, battlefieldDataOffset + 4, std::array<uint8, 16>{ 'A', 'l', 'p', 'h', 'a', 'O', 'b', 'j', 'e', 'c', 't', 'i', 'v', 'e', 'X', 'Y' }, "bar0 Name") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 20), 22, "bar1 Progress") && ok;
    ok      = expectBytes(packet, battlefieldDataOffset + 24, std::array<uint8, 16>{ 'B', 'e', 't', 'a', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, "bar1 Name") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 40), 33, "bar2 Progress") && ok;
    ok      = expectZeroRange(packet, battlefieldDataOffset + 44, battlefieldMesNumTitleOffset, "remaining bar data") && ok;
    return ok;
}

auto testScoreboard() -> bool
{
    auto packet = GP_SERV_COMMAND_BATTLEFIELD();
    packet.addScoreboard({ -7, 12 }, { 100, 200, 300, 400, 500, 600 });

    bool ok = true;
    ok      = expectEqualUInt(readU32(packet, battlefieldModeOffset), 0x1000, "scoreboard Mode") && ok;
    ok      = expectEqualUInt(packetData(packet)[battlefieldFlagsOffset], OBJECTIVEUTILITY_PROGRESS, "scoreboard Flags") && ok;
    ok      = expectEqualInt(readI32(packet, battlefieldDataOffset), -7, "MarchlandScore") && ok;
    ok      = expectEqualInt(readI32(packet, battlefieldDataOffset + 4), 12, "StrongholdScore") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 8), 100, "MarchlandProgress") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 12), 200, "MarchlandProgressMax") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 16), 300, "StrongholdProgress") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 20), 400, "StrongholdProgressMax") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 24), 500, "MarchlandNameOverride") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldDataOffset + 28), 600, "StrongholdNameOverride") && ok;
    ok      = expectZeroRange(packet, battlefieldDataOffset + 32, battlefieldMesNumTitleOffset, "scoreboard padding") && ok;
    return ok;
}

auto testFenceAndHelpText() -> bool
{
    auto packet = GP_SERV_COMMAND_BATTLEFIELD();
    packet.addFence(1.5f, -2.25f, 3.75f, 4.125f, true);
    packet.addHelpText(0x1234, 0x0021);

    bool ok = true;
    ok      = expectEqualUInt(packetData(packet)[battlefieldFlagsOffset], OBJECTIVEUTILITY_FENCE | OBJECTIVEUTILITY_HELP, "fence/help Flags") && ok;
    ok      = expectEqualInt(readI32(packet, battlefieldFenceXOffset), 1500, "FenceX") && ok;
    ok      = expectEqualInt(readI32(packet, battlefieldFenceYOffset), -2250, "FenceY") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldFenceRadiusOffset), 3750, "FenceRadius") && ok;
    ok      = expectEqualUInt(readU32(packet, battlefieldFenceRotationOffset), 4125, "FenceRotation") && ok;
    ok      = expectEqualUInt(packetData(packet)[battlefieldFenceColorOffset], 1, "FenceColor") && ok;
    ok      = expectEqualUInt(readU16(packet, battlefieldMesNumTitleOffset), 0x1234, "MesNumTitle") && ok;
    ok      = expectEqualUInt(readU16(packet, battlefieldMesNumDescriptionOffset), 14, "MesNumDescription") && ok;

    auto lowDescriptionPacket = GP_SERV_COMMAND_BATTLEFIELD();
    lowDescriptionPacket.addHelpText(0x4321, 18);
    ok = expectEqualUInt(readU16(lowDescriptionPacket, battlefieldMesNumTitleOffset), 0x4321, "low description title") && ok;
    ok = expectEqualUInt(readU16(lowDescriptionPacket, battlefieldMesNumDescriptionOffset), 0, "low description") && ok;
    return ok;
}

} // namespace

auto runS2CBattlefieldPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testDefaultConstructor() && ok;
    ok      = testCountdown() && ok;
    ok      = testBars() && ok;
    ok      = testScoreboard() && ok;
    ok      = testFenceAndHelpText() && ok;
    return ok;
}
