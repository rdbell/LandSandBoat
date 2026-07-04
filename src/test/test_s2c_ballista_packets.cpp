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

#include "test_s2c_ballista_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0e6_ballista.h"

namespace
{

using ScoreboardPacketData = GP_SERV_COMMAND_BALLISTA::SCOREBOARD::PacketData;
using ScoutPacketData      = GP_SERV_COMMAND_BALLISTA::SCOUT::PacketData;

constexpr auto scoreboardModeFlagsOffset = sizeof(GP_SERV_HEADER);
constexpr auto scoreboardPadding06Offset = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, padding06);
constexpr auto scoreboardPetraCountOffset = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, PetraCount);
constexpr auto scoreboardScoreOffset     = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, Score);
constexpr auto scoreboardScoreboardOffset = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, Scoreboard);
constexpr auto scoreboardMatchPointsOffset = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, MatchPoints);
constexpr auto scoreboardMatchSetOffset  = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, MatchSet);
constexpr auto scoreboardFlammesOffset   = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, Flammes);
constexpr auto scoreboardFlammeFlgOffset = sizeof(GP_SERV_HEADER) + offsetof(ScoreboardPacketData, FlammeFlg);
constexpr auto scoreboardPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(ScoreboardPacketData);

constexpr auto scoutModeFlagsOffset      = sizeof(GP_SERV_HEADER);
constexpr auto scoutPadding06Offset      = sizeof(GP_SERV_HEADER) + offsetof(ScoutPacketData, padding06);
constexpr auto scoutRookUniqueNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(ScoutPacketData, RookUniqueNo);
constexpr auto scoutPadding0COffset      = sizeof(GP_SERV_HEADER) + offsetof(ScoutPacketData, padding0C);
constexpr auto scoutRookPositionOffset   = sizeof(GP_SERV_HEADER) + offsetof(ScoutPacketData, RookPosition);
constexpr auto scoutRookDistanceOffset   = sizeof(GP_SERV_HEADER) + offsetof(ScoutPacketData, RookDistance);
constexpr auto scoutPacketSize           = sizeof(GP_SERV_HEADER) + sizeof(ScoutPacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c BALLISTA packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c BALLISTA packet self-test failed: " << label << " got";
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
            std::cerr << "s2c BALLISTA packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;

    ok = expectEqualUInt(sizeof(ScoreboardPacketData), 36, "SCOREBOARD sizeof(PacketData)") && ok;
    ok = expectEqualUInt(scoreboardPacketSize, 40, "SCOREBOARD packet size") && ok;
    ok = expectEqualUInt(scoreboardModeFlagsOffset, 4, "SCOREBOARD Flags/Mode offset") && ok;
    ok = expectEqualUInt(scoreboardPadding06Offset, 6, "SCOREBOARD padding06 offset") && ok;
    ok = expectEqualUInt(scoreboardPetraCountOffset, 8, "SCOREBOARD PetraCount offset") && ok;
    ok = expectEqualUInt(scoreboardScoreOffset, 12, "SCOREBOARD Score offset") && ok;
    ok = expectEqualUInt(scoreboardScoreboardOffset, 24, "SCOREBOARD Scoreboard offset") && ok;
    ok = expectEqualUInt(scoreboardMatchPointsOffset, 28, "SCOREBOARD MatchPoints offset") && ok;
    ok = expectEqualUInt(scoreboardMatchSetOffset, 31, "SCOREBOARD MatchSet offset") && ok;
    ok = expectEqualUInt(scoreboardFlammesOffset, 32, "SCOREBOARD Flammes offset") && ok;
    ok = expectEqualUInt(scoreboardFlammeFlgOffset, 36, "SCOREBOARD FlammeFlg offset") && ok;

    ok = expectEqualUInt(sizeof(ScoutPacketData), 32, "SCOUT sizeof(PacketData)") && ok;
    ok = expectEqualUInt(scoutPacketSize, 36, "SCOUT packet size") && ok;
    ok = expectEqualUInt(scoutModeFlagsOffset, 4, "SCOUT Flags/Mode offset") && ok;
    ok = expectEqualUInt(scoutPadding06Offset, 6, "SCOUT padding06 offset") && ok;
    ok = expectEqualUInt(scoutRookUniqueNoOffset, 8, "SCOUT RookUniqueNo offset") && ok;
    ok = expectEqualUInt(scoutPadding0COffset, 12, "SCOUT padding0C offset") && ok;
    ok = expectEqualUInt(scoutRookPositionOffset, 16, "SCOUT RookPosition offset") && ok;
    ok = expectEqualUInt(scoutRookDistanceOffset, 32, "SCOUT RookDistance offset") && ok;
    return ok;
}

auto testModeFlagsBitfields() -> bool
{
    ScoreboardPacketData data{};
    data.Flags = 0x23;
    data.Mode  = 0x155;

    std::uint16_t raw = 0;
    std::memcpy(&raw, &data, sizeof(raw));

    ScoreboardPacketData decoded{};
    const auto           expectedRaw = std::uint16_t{ 0x5563 };
    std::memcpy(&decoded, &expectedRaw, sizeof(expectedRaw));

    bool ok = true;
    ok      = expectEqualUInt(raw, expectedRaw, "encoded Flags/Mode bits") && ok;
    ok      = expectEqualUInt(decoded.Flags, 0x23, "decoded Flags") && ok;
    ok      = expectEqualUInt(decoded.Mode, 0x155, "decoded Mode") && ok;
    return ok;
}

auto testScoreboardConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_BALLISTA::SCOREBOARD();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0xE6, 0x14, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0E6, "SCOREBOARD type") && ok;
    ok      = expectEqualUInt(packet.getSize(), scoreboardPacketSize, "SCOREBOARD size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded SCOREBOARD prefix") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), scoreboardPacketSize, "SCOREBOARD payload") && ok;
    ok      = expectZeroRange(packet, scoreboardPacketSize, PACKET_SIZE, "SCOREBOARD tail") && ok;
    return ok;
}

auto testScoutConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_BALLISTA::SCOUT();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0xE6, 0x12, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0E6, "SCOUT type") && ok;
    ok      = expectEqualUInt(packet.getSize(), scoutPacketSize, "SCOUT size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded SCOUT prefix") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), scoutPacketSize, "SCOUT payload") && ok;
    ok      = expectZeroRange(packet, scoutPacketSize, PACKET_SIZE, "SCOUT tail") && ok;
    return ok;
}

} // namespace

auto runS2CBallistaPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testModeFlagsBitfields() && ok;
    ok      = testScoreboardConstructor() && ok;
    ok      = testScoutConstructor() && ok;
    return ok;
}
