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

#include "test_s2c_group_comlink_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0e0_group_comlink.h"

namespace
{

using GroupComlinkPacket = GP_SERV_COMMAND_GROUP_COMLINK;

constexpr auto groupComlinkLinkshellNumOffset = sizeof(GP_SERV_HEADER) + offsetof(GroupComlinkPacket::PacketData, LinkshellNum);
constexpr auto groupComlinkItemIndexOffset    = sizeof(GP_SERV_HEADER) + offsetof(GroupComlinkPacket::PacketData, ItemIndex);
constexpr auto groupComlinkCategoryOffset     = sizeof(GP_SERV_HEADER) + offsetof(GroupComlinkPacket::PacketData, Category);
constexpr auto groupComlinkPadding00Offset    = sizeof(GP_SERV_HEADER) + offsetof(GroupComlinkPacket::PacketData, padding00);
constexpr auto groupComlinkPacketDataSize     = sizeof(GroupComlinkPacket::PacketData);
constexpr auto groupComlinkPacketSize         = sizeof(GP_SERV_HEADER) + groupComlinkPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_COMLINK packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_COMLINK packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_GROUP_COMLINK), 0x0E0, "GROUP_COMLINK packet id") && ok;
    ok      = expectEqualUInt(groupComlinkPacketDataSize, 4, "sizeof(GROUP_COMLINK::PacketData)") && ok;
    ok      = expectEqualUInt(groupComlinkPacketSize, 8, "GROUP_COMLINK packet size") && ok;
    ok      = expectEqualUInt(groupComlinkLinkshellNumOffset, 4, "LinkshellNum offset") && ok;
    ok      = expectEqualUInt(groupComlinkItemIndexOffset, 5, "ItemIndex offset") && ok;
    ok      = expectEqualUInt(groupComlinkCategoryOffset, 6, "Category offset") && ok;
    ok      = expectEqualUInt(groupComlinkPadding00Offset, 7, "padding00 offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data          = GroupComlinkPacket::PacketData{};
    data.LinkshellNum = 0x01;
    data.ItemIndex    = 0x22;
    data.Category     = 0x33;
    data.padding00    = 0x44;

    auto expected = std::array<uint8, 4>{};
    expected[0]   = 0x01;
    expected[1]   = 0x22;
    expected[2]   = 0x33;
    expected[3]   = 0x44;

    return expectStructBytes(data, expected, "GROUP_COMLINK PacketData bytes");
}

} // namespace

auto runS2CGroupComlinkPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
