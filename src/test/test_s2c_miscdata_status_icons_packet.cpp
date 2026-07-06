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

#include "test_s2c_miscdata_status_icons_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>

#include "map/packets/s2c/0x063_miscdata_status_icons.h"

namespace
{

using MiscDataStatusIconsPacket = GP_SERV_COMMAND_MISCDATA::STATUS_ICONS;

constexpr auto statusIconsPacketDataSize  = sizeof(MiscDataStatusIconsPacket::PacketData);
constexpr auto statusIconsPacketSize      = sizeof(GP_SERV_HEADER) + statusIconsPacketDataSize;
constexpr auto statusIconsTypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(MiscDataStatusIconsPacket::PacketData, type);
constexpr auto statusIconsUnknown06Offset = sizeof(GP_SERV_HEADER) + offsetof(MiscDataStatusIconsPacket::PacketData, unknown06);
constexpr auto statusIconsIconsOffset     = sizeof(GP_SERV_HEADER) + offsetof(MiscDataStatusIconsPacket::PacketData, icons);
constexpr auto statusIconsTimestampsOffset = sizeof(GP_SERV_HEADER) + offsetof(MiscDataStatusIconsPacket::PacketData, timestamps);
constexpr auto statusIconsIcon31Offset     = statusIconsIconsOffset + 31 * sizeof(MiscDataStatusIconsPacket::PacketData::icons[0]);
constexpr auto statusIconsTimestamp31Offset = statusIconsTimestampsOffset + 31 * sizeof(MiscDataStatusIconsPacket::PacketData::timestamps[0]);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISCDATA STATUS_ICONS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MISCDATA STATUS_ICONS packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::StatusIcons), 0x09, "StatusIcons miscdata type") && ok;
    ok      = expectEqualUInt(statusIconsPacketDataSize, 196, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(statusIconsPacketSize, 200, "packet size") && ok;
    ok      = expectEqualUInt(statusIconsTypeOffset, 4, "type offset") && ok;
    ok      = expectEqualUInt(statusIconsUnknown06Offset, 6, "unknown06 offset") && ok;
    ok      = expectEqualUInt(statusIconsIconsOffset, 8, "icons offset") && ok;
    ok      = expectEqualUInt(statusIconsTimestampsOffset, 72, "timestamps offset") && ok;
    ok      = expectEqualUInt(statusIconsIcon31Offset, 70, "icons[31] offset") && ok;
    ok      = expectEqualUInt(statusIconsTimestamp31Offset, 196, "timestamps[31] offset") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataStatusIconsPacket::PacketData::icons), 64, "icons size") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataStatusIconsPacket::PacketData::timestamps), 128, "timestamps size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = MiscDataStatusIconsPacket::PacketData{};

    data.type      = GP_SERV_COMMAND_MISCDATA_TYPE::StatusIcons;
    data.unknown06 = statusIconsPacketDataSize;
    std::ranges::fill(data.icons, 0x00FF);
    data.icons[0]       = 0x0102;
    data.icons[1]       = 0x0304;
    data.icons[31]      = 0x3132;
    data.timestamps[0]  = 0x7FFFFFFF;
    data.timestamps[1]  = 0x11223344;
    data.timestamps[31] = 0x51525354;

    auto expected = std::array<uint8, statusIconsPacketDataSize>{};
    putLE16(expected, 0, 0x09);
    putLE16(expected, 2, statusIconsPacketDataSize);
    putLE16(expected, 4, 0x0102);
    putLE16(expected, 6, 0x0304);
    for (std::size_t i = 2; i < std::size(data.icons); ++i)
    {
        putLE16(expected, 4 + 2 * i, 0x00FF);
    }
    putLE16(expected, 66, 0x3132);
    putLE32(expected, 68, 0x7FFFFFFF);
    putLE32(expected, 72, 0x11223344);
    putLE32(expected, 192, 0x51525354);

    return expectStructBytes(data, expected, "STATUS_ICONS PacketData bytes");
}

} // namespace

auto runS2CMiscDataStatusIconsPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
