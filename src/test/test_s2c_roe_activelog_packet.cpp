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

#include "test_s2c_roe_activelog_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>

#include "map/packets/s2c/0x111_roe_activelog.h"

namespace
{

using ROEActiveLogPacket = GP_SERV_COMMAND_ROE_ACTIVELOG;

constexpr auto roeActiveLogRecordCount     = std::size(ROEActiveLogPacket::PacketData::records);
constexpr auto roeActiveLogRecordSize      = sizeof(record_t);
constexpr auto roeActiveLogRecordsOffset   = sizeof(GP_SERV_HEADER) + offsetof(ROEActiveLogPacket::PacketData, records);
constexpr auto roeActiveLogPacketDataSize  = sizeof(ROEActiveLogPacket::PacketData);
constexpr auto roeActiveLogPacketSize      = sizeof(GP_SERV_HEADER) + roeActiveLogPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ROE_ACTIVELOG packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ROE_ACTIVELOG packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_ROE_ACTIVELOG), 0x111, "ROE_ACTIVELOG packet id") && ok;
    ok      = expectEqualUInt(roeActiveLogRecordCount, 64, "record count") && ok;
    ok      = expectEqualUInt(roeActiveLogRecordSize, 4, "sizeof(record_t)") && ok;
    ok      = expectEqualUInt(roeActiveLogPacketDataSize, 256, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(roeActiveLogPacketSize, 260, "packet size") && ok;
    ok      = expectEqualUInt(roeActiveLogRecordsOffset, 4, "records offset") && ok;
    return ok;
}

auto testRecordBytes() -> bool
{
    auto data = ROEActiveLogPacket::PacketData{};
    data.records[0]  = record_t{ .Id = 0x0ABC, .Count = 0x54321 };
    data.records[63] = record_t{ .Id = 0x0FFF, .Count = 0xFFFFF };

    auto expected = std::array<uint8, roeActiveLogPacketDataSize>{};
    expected[0]   = 0xBC;
    expected[1]   = 0x1A;
    expected[2]   = 0x32;
    expected[3]   = 0x54;
    expected[252] = 0xFF;
    expected[253] = 0xFF;
    expected[254] = 0xFF;
    expected[255] = 0xFF;

    return expectStructBytes(data, expected, "ROE_ACTIVELOG PacketData bytes");
}

} // namespace

auto runS2CROEActiveLogPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testRecordBytes() && ok;
    return ok;
}
