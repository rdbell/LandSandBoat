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

#include "test_s2c_job_points_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x08d_job_points.h"

namespace
{

using JobPointsPacket = GP_SERV_COMMAND_JOB_POINTS;

constexpr auto jobPointsFirstOffset      = sizeof(GP_SERV_HEADER) + offsetof(JobPointsPacket::PacketData, points);
constexpr auto jobPointsLastOffset       = jobPointsFirstOffset + (63 * sizeof(jobpoint_t));
constexpr auto jobPointsPacketDataSize   = sizeof(JobPointsPacket::PacketData);
constexpr auto jobPointsPacketSize       = sizeof(GP_SERV_HEADER) + jobPointsPacketDataSize;
constexpr auto jobPointsPacketEntryCount = sizeof(JobPointsPacket::PacketData{}.points) / sizeof(jobpoint_t);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c JOB_POINTS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c JOB_POINTS packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_JOB_POINTS), 0x08D, "JOB_POINTS packet id") && ok;
    ok      = expectEqualUInt(sizeof(jobpoint_t), 4, "sizeof(jobpoint_t)") && ok;
    ok      = expectEqualUInt(jobPointsPacketEntryCount, 64, "JOB_POINTS entry count") && ok;
    ok      = expectEqualUInt(jobPointsPacketDataSize, 256, "sizeof(JOB_POINTS::PacketData)") && ok;
    ok      = expectEqualUInt(jobPointsPacketSize, 260, "JOB_POINTS packet size") && ok;
    ok      = expectEqualUInt(jobPointsFirstOffset, 4, "points[0] offset") && ok;
    ok      = expectEqualUInt(jobPointsLastOffset, 256, "points[63] offset") && ok;
    return ok;
}

auto testJobPointEntryBytes() -> bool
{
    auto entry   = jobpoint_t{};
    entry.index  = 0x12;
    entry.job_no = 0x345;
    entry.next   = 0x2AB;
    entry.level  = 0x2D;

    return expectStructBytes(entry, std::array<uint8, 4>{0xB2, 0x68, 0xAB, 0xB6}, "jobpoint_t bytes");
}

auto testPacketDataBytes() -> bool
{
    auto data = JobPointsPacket::PacketData{};

    data.points[0].index   = 0x12;
    data.points[0].job_no  = 0x345;
    data.points[0].next    = 0x2AB;
    data.points[0].level   = 0x2D;
    data.points[63].index  = 0x1F;
    data.points[63].job_no = 0x7FF;
    data.points[63].next   = 0x3FF;
    data.points[63].level  = 0x3F;

    auto expected = std::array<uint8, 256>{};
    expected[0]   = 0xB2;
    expected[1]   = 0x68;
    expected[2]   = 0xAB;
    expected[3]   = 0xB6;
    expected[252] = 0xFF;
    expected[253] = 0xFF;
    expected[254] = 0xFF;
    expected[255] = 0xFF;

    return expectStructBytes(data, expected, "JOB_POINTS PacketData bytes");
}

} // namespace

auto runS2CJobPointsPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testJobPointEntryBytes() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
