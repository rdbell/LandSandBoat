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

#include "test_s2c_mount_data_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0ae_mount_data.h"

namespace
{

using MountDataPacket = GP_SERV_COMMAND_MOUNT_DATA;

constexpr auto mountDataTblOffset      = sizeof(GP_SERV_HEADER) + offsetof(MountDataPacket::PacketData, MountDataTbl);
constexpr auto mountDataTblSize        = sizeof(MountDataPacket::PacketData{}.MountDataTbl);
constexpr auto mountDataPacketDataSize = sizeof(MountDataPacket::PacketData);
constexpr auto mountDataPacketSize     = sizeof(GP_SERV_HEADER) + mountDataPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MOUNT_DATA packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MOUNT_DATA packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MOUNT_DATA), 0x0AE, "MOUNT_DATA packet id") && ok;
    ok      = expectEqualUInt(mountDataTblSize, 8, "MountDataTbl size") && ok;
    ok      = expectEqualUInt(mountDataPacketDataSize, 8, "sizeof(MOUNT_DATA::PacketData)") && ok;
    ok      = expectEqualUInt(mountDataPacketSize, 12, "MOUNT_DATA packet size") && ok;
    ok      = expectEqualUInt(mountDataTblOffset, 4, "MountDataTbl offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data             = MountDataPacket::PacketData{};
    data.MountDataTbl[0] = 0x12;
    data.MountDataTbl[7] = 0x78;

    auto expected = std::array<uint8, 8>{};
    expected[0]   = 0x12;
    expected[7]   = 0x78;

    return expectStructBytes(data, expected, "MOUNT_DATA PacketData bytes");
}

} // namespace

auto runS2CMountDataPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
