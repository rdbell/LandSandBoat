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

#include "test_s2c_unity_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x110_unity.h"

namespace
{

using UnityPacket = GP_SERV_COMMAND_UNITY;

constexpr auto unitySparksOffset    = sizeof(GP_SERV_HEADER);
constexpr auto unityDeedsOffset     = sizeof(GP_SERV_HEADER) + offsetof(UnityPacket::PacketData, Deeds);
constexpr auto unityPlauditsOffset  = sizeof(GP_SERV_HEADER) + offsetof(UnityPacket::PacketData, Plaudits);
constexpr auto unitySharedOffset    = sizeof(GP_SERV_HEADER) + offsetof(UnityPacket::PacketData, RoEUnityShared);
constexpr auto unityLeaderOffset    = sizeof(GP_SERV_HEADER) + offsetof(UnityPacket::PacketData, RoEUnityLeader);
constexpr auto unityUnknown0EOffset = sizeof(GP_SERV_HEADER) + offsetof(UnityPacket::PacketData, unknown0E);
constexpr auto unityUnknown0ESize   = sizeof(UnityPacket::PacketData::unknown0E);
constexpr auto unityPacketDataSize  = sizeof(UnityPacket::PacketData);
constexpr auto unityPacketSize      = sizeof(GP_SERV_HEADER) + unityPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c UNITY packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c UNITY packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_UNITY), 0x110, "UNITY packet id") && ok;
    ok      = expectEqualUInt(unityPacketDataSize, 16, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(unityPacketSize, 20, "packet size") && ok;
    ok      = expectEqualUInt(unitySparksOffset, 4, "Sparks offset") && ok;
    ok      = expectEqualUInt(unityDeedsOffset, 8, "Deeds offset") && ok;
    ok      = expectEqualUInt(unityPlauditsOffset, 10, "Plaudits offset") && ok;
    ok      = expectEqualUInt(unitySharedOffset, 12, "RoEUnityShared offset") && ok;
    ok      = expectEqualUInt(unityLeaderOffset, 13, "RoEUnityLeader offset") && ok;
    ok      = expectEqualUInt(unityUnknown0EOffset, 14, "unknown0E offset") && ok;
    ok      = expectEqualUInt(unityUnknown0ESize, 6, "unknown0E size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = UnityPacket::PacketData{};

    data.Sparks         = 0x123456;
    data.unused00       = 0xAB;
    data.Deeds          = 0xBEEF;
    data.Plaudits       = 0xCAFE;
    data.RoEUnityShared = 5;
    data.RoEUnityLeader = 3;
    std::memset(data.unknown0E, 0xFF, sizeof(data.unknown0E));

    auto expected = std::array<uint8, unityPacketDataSize>{
        0x56, 0x34, 0x12, 0xAB,
        0xEF, 0xBE,
        0xFE, 0xCA,
        0x05,
        0x03,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    };

    return expectStructBytes(data, expected, "UNITY PacketData bytes");
}

} // namespace

auto runS2CUnityPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
