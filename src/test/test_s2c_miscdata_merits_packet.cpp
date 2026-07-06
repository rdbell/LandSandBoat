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

#include "test_s2c_miscdata_merits_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x063_miscdata_merits.h"

namespace
{

using MiscDataMeritsPacket = GP_SERV_COMMAND_MISCDATA::MERITS;

constexpr auto meritsPacketDataSize      = sizeof(MiscDataMeritsPacket::PacketData);
constexpr auto meritsPacketSize          = sizeof(GP_SERV_HEADER) + meritsPacketDataSize;
constexpr auto meritsTypeOffset          = sizeof(GP_SERV_HEADER) + offsetof(MiscDataMeritsPacket::PacketData, type);
constexpr auto meritsUnknown06Offset     = sizeof(GP_SERV_HEADER) + offsetof(MiscDataMeritsPacket::PacketData, unknown06);
constexpr auto meritsLimitPointsOffset   = sizeof(GP_SERV_HEADER) + offsetof(MiscDataMeritsPacket::PacketData, limitPoints);
constexpr auto meritsMaxMeritPointOffset = sizeof(GP_SERV_HEADER) + offsetof(MiscDataMeritsPacket::PacketData, maxMeritPoints);
constexpr auto meritsPaddingOffset       = sizeof(GP_SERV_HEADER) + offsetof(MiscDataMeritsPacket::PacketData, padding);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISCDATA MERITS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MISCDATA MERITS packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MISCDATA), 0x063, "MISCDATA packet id") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::Merits), 0x02, "Merits miscdata type") && ok;
    ok      = expectEqualUInt(meritsPacketDataSize, 12, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(meritsPacketSize, 16, "packet size") && ok;
    ok      = expectEqualUInt(meritsTypeOffset, 4, "type offset") && ok;
    ok      = expectEqualUInt(meritsUnknown06Offset, 6, "unknown06 offset") && ok;
    ok      = expectEqualUInt(meritsLimitPointsOffset, 8, "limitPoints offset") && ok;
    ok      = expectEqualUInt(meritsMaxMeritPointOffset, 12, "maxMeritPoints offset") && ok;
    ok      = expectEqualUInt(meritsPaddingOffset, 13, "padding offset") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataMeritsPacket::PacketData::padding), 3, "padding size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = MiscDataMeritsPacket::PacketData{};

    data.type                 = GP_SERV_COMMAND_MISCDATA_TYPE::Merits;
    data.unknown06            = meritsPacketDataSize;
    data.limitPoints          = 0x1234;
    data.meritPoints          = 0x55;
    data.bluBonus             = 0x2D;
    data.canUseMeritMode      = 1;
    data.xpCappedOrMeritMode  = 0;
    data.meritModeEnabled     = 1;
    data.maxMeritPoints       = 0x2A;

    auto expected = std::array<uint8, meritsPacketDataSize>{};
    putLE16(expected, 0, 0x02);
    putLE16(expected, 2, meritsPacketDataSize);
    putLE16(expected, 4, 0x1234);
    putLE16(expected, 6, 0xB6D5);
    expected[8] = 0x2A;

    return expectStructBytes(data, expected, "MERITS PacketData bytes");
}

} // namespace

auto runS2CMiscDataMeritsPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
