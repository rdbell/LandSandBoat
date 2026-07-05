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

#include "test_s2c_group_checkid_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0e1_group_checkid.h"

namespace
{

using GroupCheckIDPacket = GP_SERV_COMMAND_GROUP_CHECKID;

constexpr auto groupCheckIDGroupIDOffset     = sizeof(GP_SERV_HEADER) + offsetof(GroupCheckIDPacket::PacketData, GroupID);
constexpr auto groupCheckIDPacketDataSize    = sizeof(GroupCheckIDPacket::PacketData);
constexpr auto groupCheckIDPacketSize        = sizeof(GP_SERV_HEADER) + groupCheckIDPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_CHECKID packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_CHECKID packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_GROUP_CHECKID), 0x0E1, "GROUP_CHECKID packet id") && ok;
    ok      = expectEqualUInt(groupCheckIDPacketDataSize, 4, "sizeof(GROUP_CHECKID::PacketData)") && ok;
    ok      = expectEqualUInt(groupCheckIDPacketSize, 8, "GROUP_CHECKID packet size") && ok;
    ok      = expectEqualUInt(groupCheckIDGroupIDOffset, 4, "GroupID offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data    = GroupCheckIDPacket::PacketData{};
    data.GroupID = 0x11223344;

    auto expected = std::array<uint8, 4>{};
    expected[0]   = 0x44;
    expected[1]   = 0x33;
    expected[2]   = 0x22;
    expected[3]   = 0x11;

    return expectStructBytes(data, expected, "GROUP_CHECKID PacketData bytes");
}

auto testNullConstructorBytes() -> bool
{
    const auto packet = GroupCheckIDPacket(nullptr);
    return expectEqualUInt(packet.ref<uint32>(groupCheckIDGroupIDOffset), 0, "nullptr constructor GroupID");
}

} // namespace

auto runS2CGroupCheckIDPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testNullConstructorBytes() && ok;
    return ok;
}
