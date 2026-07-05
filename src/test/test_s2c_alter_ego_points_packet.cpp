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

#include "test_s2c_alter_ego_points_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x08e_alter_ego_points.h"

namespace
{

using AlterEgoPacket = GP_SERV_PACKET_ALTER_EGO_POINTS;

constexpr auto alterEgoPointsOffset   = sizeof(GP_SERV_HEADER) + offsetof(AlterEgoPacket::PacketData, Points);
constexpr auto alterEgoPaddingOffset  = sizeof(GP_SERV_HEADER) + offsetof(AlterEgoPacket::PacketData, padding00);
constexpr auto alterEgoUpgradesOffset = sizeof(GP_SERV_HEADER) + offsetof(AlterEgoPacket::PacketData, Upgrades);
constexpr auto alterEgoCostsOffset    = sizeof(GP_SERV_HEADER) + offsetof(AlterEgoPacket::PacketData, Costs);
constexpr auto alterEgoPacketDataSize = sizeof(AlterEgoPacket::PacketData);
constexpr auto alterEgoPacketSize     = sizeof(GP_SERV_HEADER) + alterEgoPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ALTER_EGO_POINTS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ALTER_EGO_POINTS packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_PACKET_ALTER_EGO_POINTS), 0x08E, "ALTER_EGO_POINTS packet id") && ok;
    ok      = expectEqualUInt(alterEgoPacketDataSize, 100, "sizeof(ALTER_EGO_POINTS::PacketData)") && ok;
    ok      = expectEqualUInt(alterEgoPacketSize, 104, "ALTER_EGO_POINTS packet size") && ok;
    ok      = expectEqualUInt(alterEgoPointsOffset, 4, "Points offset") && ok;
    ok      = expectEqualUInt(alterEgoPaddingOffset, 6, "padding00 offset") && ok;
    ok      = expectEqualUInt(alterEgoUpgradesOffset, 8, "Upgrades offset") && ok;
    ok      = expectEqualUInt(alterEgoCostsOffset, 40, "Costs offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data          = AlterEgoPacket::PacketData{};
    data.Points       = 0x1234;
    data.Upgrades[0]  = 0x05;
    data.Upgrades[31] = 0x1F;
    data.Costs[0]     = 0x2345;
    data.Costs[31]    = 0x6789;

    auto expected = std::array<uint8, 100>{};
    expected[0]   = 0x34;
    expected[1]   = 0x12;
    expected[4]   = 0x05;
    expected[35]  = 0x1F;
    expected[36]  = 0x45;
    expected[37]  = 0x23;
    expected[98]  = 0x89;
    expected[99]  = 0x67;

    return expectStructBytes(data, expected, "ALTER_EGO_POINTS PacketData bytes");
}

} // namespace

auto runS2CAlterEgoPointsPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
