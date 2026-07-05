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

#include "test_s2c_roe_log_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x112_roe_log.h"

namespace
{

using ROELogPacket = GP_SERV_COMMAND_ROE_LOG;

constexpr auto roeLogDataOffset      = sizeof(GP_SERV_HEADER) + offsetof(ROELogPacket::PacketData, Data);
constexpr auto roeLogOffsetOffset    = sizeof(GP_SERV_HEADER) + offsetof(ROELogPacket::PacketData, Offset);
constexpr auto roeLogPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(ROELogPacket::PacketData, padding00);
constexpr auto roeLogDataSize        = sizeof(ROELogPacket::PacketData::Data);
constexpr auto roeLogPacketDataSize  = sizeof(ROELogPacket::PacketData);
constexpr auto roeLogPacketSize      = sizeof(GP_SERV_HEADER) + roeLogPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ROE_LOG packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ROE_LOG packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_ROE_LOG), 0x112, "ROE_LOG packet id") && ok;
    ok      = expectEqualUInt(roeLogDataSize, 128, "Data size") && ok;
    ok      = expectEqualUInt(roeLogPacketDataSize, 132, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(roeLogPacketSize, 136, "packet size") && ok;
    ok      = expectEqualUInt(roeLogDataOffset, 4, "Data offset") && ok;
    ok      = expectEqualUInt(roeLogOffsetOffset, 132, "Offset offset") && ok;
    ok      = expectEqualUInt(roeLogPadding00Offset, 134, "padding00 offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = ROELogPacket::PacketData{};
    for (std::size_t i = 0; i < roeLogDataSize; ++i)
    {
        data.Data[i] = static_cast<uint8>(i ^ 0x5A);
    }
    data.Offset    = 0x1234;
    data.padding00 = 0xBEEF;

    auto expected = std::array<uint8, roeLogPacketDataSize>{};
    for (std::size_t i = 0; i < roeLogDataSize; ++i)
    {
        expected[i] = static_cast<uint8>(i ^ 0x5A);
    }
    expected[128] = 0x34;
    expected[129] = 0x12;
    expected[130] = 0xEF;
    expected[131] = 0xBE;

    return expectStructBytes(data, expected, "ROE_LOG PacketData bytes");
}

} // namespace

auto runS2CROELogPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
