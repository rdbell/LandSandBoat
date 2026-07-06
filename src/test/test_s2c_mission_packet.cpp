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

#include "test_s2c_mission_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x056_mission.h"
#include "map/packets/s2c/0x056_mission_other.h"
#include "map/packets/s2c/0x056_mission_tvr.h"

namespace
{

using MissionPacket      = GP_SERV_COMMAND_MISSION::MISSION;
using MissionOtherPacket = GP_SERV_COMMAND_MISSION::OTHER;
using MissionTVRPacket   = GP_SERV_COMMAND_MISSION::TVR;

constexpr auto missionPacketDataSize = sizeof(MissionPacket::PacketData);
constexpr auto missionPacketSize     = sizeof(GP_SERV_HEADER) + missionPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISSION packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <typename T, std::size_t Size>
auto expectStructBytes(const T& value, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = reinterpret_cast<const uint8*>(&value);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c MISSION packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto valueByte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(valueByte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
void putLE16(std::array<uint8, Size>& buffer, std::size_t offset, std::uint16_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
}

template <std::size_t Size>
void putLE32(std::array<uint8, Size>& buffer, std::size_t offset, std::uint32_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    buffer[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    buffer[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MISSION), 0x056, "MISSION packet id") && ok;
    ok      = expectEqualUInt(sizeof(expansion_addon_t), 2, "sizeof(expansion_addon_t)") && ok;
    ok      = expectEqualUInt(sizeof(tales_beginning_t), 2, "sizeof(tales_beginning_t)") && ok;
    ok      = expectEqualUInt(sizeof(MissionPacket::PacketData), 36, "sizeof(MISSION PacketData)") && ok;
    ok      = expectEqualUInt(sizeof(MissionOtherPacket::PacketData), 36, "sizeof(OTHER PacketData)") && ok;
    ok      = expectEqualUInt(sizeof(MissionTVRPacket::PacketData), 36, "sizeof(TVR PacketData)") && ok;
    ok      = expectEqualUInt(missionPacketSize, 40, "packet size") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionPacket::PacketData, Nation), 4, "MISSION Nation offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionPacket::PacketData, Expansion_Addons), 24, "MISSION Expansion_Addons offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionPacket::PacketData, TalesBeginning), 26, "MISSION TalesBeginning offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionPacket::PacketData, Expansion_SoA), 28, "MISSION Expansion_SoA offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionPacket::PacketData, Expansion_RoV), 32, "MISSION Expansion_RoV offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionPacket::PacketData, Port), 36, "MISSION Port offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionPacket::PacketData, padding26), 38, "MISSION padding26 offset") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionOtherPacket::PacketData, Data), 4, "OTHER Data offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionOtherPacket::PacketData, Port), 36, "OTHER Port offset") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionTVRPacket::PacketData, Expansion_TVR), 4, "TVR Expansion_TVR offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionTVRPacket::PacketData, padding08), 8, "TVR padding08 offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(MissionTVRPacket::PacketData, Port), 36, "TVR Port offset") && ok;
    return ok;
}

auto testMainPacketDataBytes() -> bool
{
    auto data = MissionPacket::PacketData{};

    data.Nation           = 0x01020304;
    data.NationMission    = 0x11121314;
    data.Expansion_RotZ   = 0x21222324;
    data.Expansion_CoP    = 0x31323334;
    data.Expansion_CoP2   = 0x41424344;
    data.Expansion_Addons = {
        .ACP     = 1,
        .AMK     = 2,
        .ASA     = 3,
        .padding = 0xF,
    };
    data.TalesBeginning = {
        .RoTZ = 1,
        .ACP  = 1,
        .ASA  = 0,
        .CoP  = 1,
        .SoA  = 1,
        .RoV  = 0,
    };
    data.Expansion_SoA = 0x51525354;
    data.Expansion_RoV = 0x61626364;
    data.Port          = 0xFFFF;
    data.padding26     = 0xBEEF;

    auto expected = std::array<uint8, missionPacketDataSize>{};
    putLE32(expected, 0, 0x01020304);
    putLE32(expected, 4, 0x11121314);
    putLE32(expected, 8, 0x21222324);
    putLE32(expected, 12, 0x31323334);
    putLE32(expected, 16, 0x41424344);
    putLE16(expected, 20, 0xF321);
    putLE16(expected, 22, 0x002B);
    putLE32(expected, 24, 0x51525354);
    putLE32(expected, 28, 0x61626364);
    putLE16(expected, 32, 0xFFFF);
    putLE16(expected, 34, 0xBEEF);

    return expectStructBytes(data, expected, "MISSION PacketData bytes");
}

auto testOtherPacketDataBytes() -> bool
{
    auto data = MissionOtherPacket::PacketData{};

    data.Data[0]   = 0x01020304;
    data.Data[3]   = 0x31323334;
    data.Data[7]   = 0x71727374;
    data.Port      = 0x1234;
    data.padding26 = 0xBEEF;

    auto expected = std::array<uint8, missionPacketDataSize>{};
    putLE32(expected, 0, 0x01020304);
    putLE32(expected, 12, 0x31323334);
    putLE32(expected, 28, 0x71727374);
    putLE16(expected, 32, 0x1234);
    putLE16(expected, 34, 0xBEEF);

    return expectStructBytes(data, expected, "MISSION OTHER PacketData bytes");
}

auto testTVRPacketDataBytes() -> bool
{
    auto data = MissionTVRPacket::PacketData{};

    data.Expansion_TVR = 999;
    data.padding08[0]  = 0x11121314;
    data.padding08[6]  = 0x71727374;
    data.Port          = 0xFFFE;
    data.padding26     = 0xBEEF;

    auto expected = std::array<uint8, missionPacketDataSize>{};
    putLE32(expected, 0, 999);
    putLE32(expected, 4, 0x11121314);
    putLE32(expected, 28, 0x71727374);
    putLE16(expected, 32, 0xFFFE);
    putLE16(expected, 34, 0xBEEF);

    return expectStructBytes(data, expected, "MISSION TVR PacketData bytes");
}

} // namespace

auto runS2CMissionPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testMainPacketDataBytes() && ok;
    ok      = testOtherPacketDataBytes() && ok;
    ok      = testTVRPacketDataBytes() && ok;
    return ok;
}
