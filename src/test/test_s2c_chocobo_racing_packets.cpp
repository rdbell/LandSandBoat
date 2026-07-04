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

#include "test_s2c_chocobo_racing_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x069_chocobo_racing.h"

namespace
{

using RacingParamsPacketData  = GP_SERV_COMMAND_CHOCOBO_RACING::RACINGPARAMS::PacketData;
using ChocoboParamsPacketData = GP_SERV_COMMAND_CHOCOBO_RACING::CHOCOBOPARAMS::PacketData;
using SectionParamsPacketData = GP_SERV_COMMAND_CHOCOBO_RACING::SECTIONPARAMS::PacketData;
using ResultParamsPacketData  = GP_SERV_COMMAND_CHOCOBO_RACING::RESULTPARAMS::PacketData;
using EndPacketData           = GP_SERV_COMMAND_CHOCOBO_RACING::END::PacketData;

constexpr auto chocoboRacingPacketSize = sizeof(GP_SERV_HEADER) + sizeof(RacingParamsPacketData);

constexpr auto racingModeOffset       = sizeof(GP_SERV_HEADER) + offsetof(RacingParamsPacketData, Mode);
constexpr auto racingPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(RacingParamsPacketData, padding00);
constexpr auto racingRaceParamsOffset = sizeof(GP_SERV_HEADER) + offsetof(RacingParamsPacketData, RaceParams);
constexpr auto racingJunk00Offset     = sizeof(GP_SERV_HEADER) + offsetof(RacingParamsPacketData, junk00);

constexpr auto chocoboModeOffset       = sizeof(GP_SERV_HEADER) + offsetof(ChocoboParamsPacketData, Mode);
constexpr auto chocoboParamIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(ChocoboParamsPacketData, ParamIndex);
constexpr auto chocoboParamSizeOffset  = sizeof(GP_SERV_HEADER) + offsetof(ChocoboParamsPacketData, ParamSize);
constexpr auto chocoboPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(ChocoboParamsPacketData, padding00);
constexpr auto chocoboParamDataOffset  = sizeof(GP_SERV_HEADER) + offsetof(ChocoboParamsPacketData, ParamData);

constexpr auto sectionModeOffset       = sizeof(GP_SERV_HEADER) + offsetof(SectionParamsPacketData, Mode);
constexpr auto sectionParamIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(SectionParamsPacketData, ParamIndex);
constexpr auto sectionParamSizeOffset  = sizeof(GP_SERV_HEADER) + offsetof(SectionParamsPacketData, ParamSize);
constexpr auto sectionPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(SectionParamsPacketData, padding00);
constexpr auto sectionParamDataOffset  = sizeof(GP_SERV_HEADER) + offsetof(SectionParamsPacketData, ParamData);

constexpr auto resultModeOffset      = sizeof(GP_SERV_HEADER) + offsetof(ResultParamsPacketData, Mode);
constexpr auto resultPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(ResultParamsPacketData, padding00);
constexpr auto resultParamSizeOffset = sizeof(GP_SERV_HEADER) + offsetof(ResultParamsPacketData, ParamSize);
constexpr auto resultPadding01Offset = sizeof(GP_SERV_HEADER) + offsetof(ResultParamsPacketData, padding01);
constexpr auto resultParamDataOffset = sizeof(GP_SERV_HEADER) + offsetof(ResultParamsPacketData, ParamData);

constexpr auto endModeOffset      = sizeof(GP_SERV_HEADER) + offsetof(EndPacketData, Mode);
constexpr auto endPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(EndPacketData, padding00);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CHOCOBO_RACING packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CHOCOBO_RACING packet self-test failed: " << label << " got";
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
            std::cerr << "s2c CHOCOBO_RACING packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

template <typename Packet>
auto testDefaultConstructor(const std::string& label) -> bool
{
    auto packet = Packet();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0x69, 0x64, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x069, label + " type") && ok;
    ok      = expectEqualUInt(packet.getSize(), chocoboRacingPacketSize, label + " size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded " + label + " prefix") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), chocoboRacingPacketSize, label + " payload") && ok;
    ok      = expectZeroRange(packet, chocoboRacingPacketSize, PACKET_SIZE, label + " tail") && ok;
    return ok;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;

    ok = expectEqualUInt(sizeof(RacingParamsPacketData), 196, "RACINGPARAMS sizeof(PacketData)") && ok;
    ok = expectEqualUInt(sizeof(ChocoboParamsPacketData), 196, "CHOCOBOPARAMS sizeof(PacketData)") && ok;
    ok = expectEqualUInt(sizeof(SectionParamsPacketData), 196, "SECTIONPARAMS sizeof(PacketData)") && ok;
    ok = expectEqualUInt(sizeof(ResultParamsPacketData), 196, "RESULTPARAMS sizeof(PacketData)") && ok;
    ok = expectEqualUInt(sizeof(EndPacketData), 196, "END sizeof(PacketData)") && ok;
    ok = expectEqualUInt(chocoboRacingPacketSize, 200, "packet size") && ok;

    ok = expectEqualUInt(racingModeOffset, 4, "RACINGPARAMS Mode offset") && ok;
    ok = expectEqualUInt(racingPadding00Offset, 5, "RACINGPARAMS padding00 offset") && ok;
    ok = expectEqualUInt(racingRaceParamsOffset, 8, "RACINGPARAMS RaceParams offset") && ok;
    ok = expectEqualUInt(sizeof(RacingParamsPacketData::RaceParams), 8, "RACINGPARAMS RaceParams size") && ok;
    ok = expectEqualUInt(racingJunk00Offset, 16, "RACINGPARAMS junk00 offset") && ok;
    ok = expectEqualUInt(sizeof(RacingParamsPacketData::junk00), 184, "RACINGPARAMS junk00 size") && ok;

    ok = expectEqualUInt(chocoboModeOffset, 4, "CHOCOBOPARAMS Mode offset") && ok;
    ok = expectEqualUInt(chocoboParamIndexOffset, 5, "CHOCOBOPARAMS ParamIndex offset") && ok;
    ok = expectEqualUInt(chocoboParamSizeOffset, 6, "CHOCOBOPARAMS ParamSize offset") && ok;
    ok = expectEqualUInt(chocoboPadding00Offset, 7, "CHOCOBOPARAMS padding00 offset") && ok;
    ok = expectEqualUInt(chocoboParamDataOffset, 8, "CHOCOBOPARAMS ParamData offset") && ok;
    ok = expectEqualUInt(sizeof(ChocoboParamsPacketData::ParamData), 192, "CHOCOBOPARAMS ParamData size") && ok;

    ok = expectEqualUInt(sectionModeOffset, 4, "SECTIONPARAMS Mode offset") && ok;
    ok = expectEqualUInt(sectionParamIndexOffset, 5, "SECTIONPARAMS ParamIndex offset") && ok;
    ok = expectEqualUInt(sectionParamSizeOffset, 6, "SECTIONPARAMS ParamSize offset") && ok;
    ok = expectEqualUInt(sectionPadding00Offset, 7, "SECTIONPARAMS padding00 offset") && ok;
    ok = expectEqualUInt(sectionParamDataOffset, 8, "SECTIONPARAMS ParamData offset") && ok;
    ok = expectEqualUInt(sizeof(SectionParamsPacketData::ParamData), 192, "SECTIONPARAMS ParamData size") && ok;

    ok = expectEqualUInt(resultModeOffset, 4, "RESULTPARAMS Mode offset") && ok;
    ok = expectEqualUInt(resultPadding00Offset, 5, "RESULTPARAMS padding00 offset") && ok;
    ok = expectEqualUInt(resultParamSizeOffset, 6, "RESULTPARAMS ParamSize offset") && ok;
    ok = expectEqualUInt(resultPadding01Offset, 7, "RESULTPARAMS padding01 offset") && ok;
    ok = expectEqualUInt(resultParamDataOffset, 8, "RESULTPARAMS ParamData offset") && ok;
    ok = expectEqualUInt(sizeof(ResultParamsPacketData::ParamData), 192, "RESULTPARAMS ParamData size") && ok;

    ok = expectEqualUInt(endModeOffset, 4, "END Mode offset") && ok;
    ok = expectEqualUInt(endPadding00Offset, 5, "END padding00 offset") && ok;
    ok = expectEqualUInt(sizeof(EndPacketData::padding00), 195, "END padding00 size") && ok;
    return ok;
}

} // namespace

auto runS2CChocoboRacingPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testDefaultConstructor<GP_SERV_COMMAND_CHOCOBO_RACING::RACINGPARAMS>("RACINGPARAMS") && ok;
    ok      = testDefaultConstructor<GP_SERV_COMMAND_CHOCOBO_RACING::CHOCOBOPARAMS>("CHOCOBOPARAMS") && ok;
    ok      = testDefaultConstructor<GP_SERV_COMMAND_CHOCOBO_RACING::SECTIONPARAMS>("SECTIONPARAMS") && ok;
    ok      = testDefaultConstructor<GP_SERV_COMMAND_CHOCOBO_RACING::RESULTPARAMS>("RESULTPARAMS") && ok;
    ok      = testDefaultConstructor<GP_SERV_COMMAND_CHOCOBO_RACING::END>("END") && ok;
    return ok;
}
