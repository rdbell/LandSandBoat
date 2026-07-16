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

#include "test_s2c_miscdata_homepoints_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x063_miscdata_homepoints.h"

namespace
{

using MiscDataHomepointsPacket = GP_SERV_COMMAND_MISCDATA::HOMEPOINTS;

constexpr auto homepointsPacketDataSize     = sizeof(MiscDataHomepointsPacket::PacketData);
constexpr auto homepointsPacketSize         = sizeof(GP_SERV_HEADER) + homepointsPacketDataSize;
constexpr auto homepointsTypeOffset         = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, type);
constexpr auto homepointsUnknown06Offset    = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, unknown06);
constexpr auto homepointsHomePointOffset    = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, homePoint);
constexpr auto homepointsSurvivalOffset     = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, survivalGuide);
constexpr auto homepointsWaypointOffset     = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, waypoint);
constexpr auto homepointsTelepointOffset    = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, telepoint);
constexpr auto homepointsAtmosOffset        = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, atmos);
constexpr auto homepointsEschanPortalOffset = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, eschanPortal);
constexpr auto homepointsUnknown00Offset    = sizeof(GP_SERV_HEADER) + offsetof(MiscDataHomepointsPacket::PacketData, unknown00);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISCDATA HOMEPOINTS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MISCDATA HOMEPOINTS packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MISCDATA), 0x063, "MISCDATA packet id") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::Homepoints), 0x06, "Homepoints miscdata type") && ok;
    ok      = expectEqualUInt(homepointsPacketDataSize, 68, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(homepointsPacketSize, 72, "packet size") && ok;
    ok      = expectEqualUInt(homepointsTypeOffset, 4, "type offset") && ok;
    ok      = expectEqualUInt(homepointsUnknown06Offset, 6, "unknown06 offset") && ok;
    ok      = expectEqualUInt(homepointsHomePointOffset, 8, "homePoint offset") && ok;
    ok      = expectEqualUInt(homepointsSurvivalOffset, 24, "survivalGuide offset") && ok;
    ok      = expectEqualUInt(homepointsWaypointOffset, 40, "waypoint offset") && ok;
    ok      = expectEqualUInt(homepointsTelepointOffset, 56, "telepoint offset") && ok;
    ok      = expectEqualUInt(homepointsAtmosOffset, 60, "atmos offset") && ok;
    ok      = expectEqualUInt(homepointsEschanPortalOffset, 64, "eschanPortal offset") && ok;
    ok      = expectEqualUInt(homepointsUnknown00Offset, 68, "unknown00 offset") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataHomepointsPacket::PacketData::homePoint), 16, "homePoint size") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataHomepointsPacket::PacketData::survivalGuide), 16, "survivalGuide size") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataHomepointsPacket::PacketData::waypoint), 16, "waypoint size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = MiscDataHomepointsPacket::PacketData{};

    data.type             = GP_SERV_COMMAND_MISCDATA_TYPE::Homepoints;
    data.unknown06        = homepointsPacketDataSize;
    data.homePoint[0]     = 0x01020304;
    data.homePoint[3]     = 0x11121314;
    data.survivalGuide[0] = 0x21222324;
    data.survivalGuide[3] = 0x31323334;
    data.waypoint[0]      = 0x41424344;
    data.waypoint[3]      = 0x51525354;
    data.telepoint        = 0x61626364;
    data.atmos            = 0x71727374;
    data.eschanPortal     = 0x81828384;
    data.unknown00        = 0x91929394;

    auto expected = std::array<uint8, homepointsPacketDataSize>{};
    putLE16(expected, 0, 0x06);
    putLE16(expected, 2, homepointsPacketDataSize);
    putLE32(expected, 4, 0x01020304);
    putLE32(expected, 16, 0x11121314);
    putLE32(expected, 20, 0x21222324);
    putLE32(expected, 32, 0x31323334);
    putLE32(expected, 36, 0x41424344);
    putLE32(expected, 48, 0x51525354);
    putLE32(expected, 52, 0x61626364);
    putLE32(expected, 56, 0x71727374);
    putLE32(expected, 60, 0x81828384);
    putLE32(expected, 64, 0x91929394);

    return expectStructBytes(data, expected, "HOMEPOINTS PacketData bytes");
}

auto testConstructorCopiesActiveMasksAndZerosUnsupportedFields() -> bool
{
    auto character                         = CCharEntity{};
    character.teleport.homepoint.access[0] = 0x01020304;
    character.teleport.homepoint.access[3] = 0x11121314;
    character.teleport.survival.access[0]  = 0x21222324;
    character.teleport.survival.access[3]  = 0x31323334;
    character.teleport.waypoints.access[0] = 0x41424344;
    character.teleport.waypoints.access[1] = 0x51525354;

    auto packet           = MiscDataHomepointsPacket(&character);
    auto data             = MiscDataHomepointsPacket::PacketData{};
    data.type             = GP_SERV_COMMAND_MISCDATA_TYPE::Homepoints;
    data.unknown06        = homepointsPacketDataSize;
    data.homePoint[0]     = 0x01020304;
    data.homePoint[3]     = 0x11121314;
    data.survivalGuide[0] = 0x21222324;
    data.survivalGuide[3] = 0x31323334;
    data.waypoint[0]      = 0x41424344;
    data.waypoint[1]      = 0x51525354;

    const auto* packetBytes = reinterpret_cast<const uint8*>(&packet);
    return expectStructBytes(data, *reinterpret_cast<const std::array<uint8, homepointsPacketDataSize>*>(packetBytes + sizeof(GP_SERV_HEADER)), "constructor active masks and zeroed unsupported fields");
}

} // namespace

auto runS2CMiscDataHomepointsPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testConstructorCopiesActiveMasksAndZerosUnsupportedFields() && ok;
    return ok;
}
