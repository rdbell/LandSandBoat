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

#include "test_s2c_miscdata_job_points_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x063_miscdata_job_points.h"
#include "map/packets/s2c/miscdata_job_points_runtime.h"

namespace
{

using MiscDataJobPointsPacket = GP_SERV_COMMAND_MISCDATA::JOB_POINTS;

constexpr auto jobPointsPacketDataSize       = sizeof(MiscDataJobPointsPacket::PacketData);
constexpr auto jobPointsPacketSize           = sizeof(GP_SERV_HEADER) + jobPointsPacketDataSize;
constexpr auto jobPointsTypeOffset           = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, type);
constexpr auto jobPointsUnknown06Offset      = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, unknown06);
constexpr auto jobPointsAccessOffset         = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, access);
constexpr auto jobPointsPaddingOffset        = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, padding);
constexpr auto jobPointsJobsOffset           = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, jobs);
constexpr auto jobPointsJob1Offset           = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, jobs) + sizeof(MiscDataJobPointsPacket::PacketData::jobs[0]);
constexpr auto jobPointsJob23Offset          = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, jobs) + 23 * sizeof(MiscDataJobPointsPacket::PacketData::jobs[0]);
constexpr auto jobPointsCapacityPointsOffset = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, jobs) + offsetof(MiscDataJobPointsPacket::PacketData::JobPointData, capacityPoints);
constexpr auto jobPointsCurrentJpOffset      = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, jobs) + offsetof(MiscDataJobPointsPacket::PacketData::JobPointData, currentJp);
constexpr auto jobPointsTotalJpSpentOffset   = sizeof(GP_SERV_HEADER) + offsetof(MiscDataJobPointsPacket::PacketData, jobs) + offsetof(MiscDataJobPointsPacket::PacketData::JobPointData, totalJpSpent);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISCDATA JOB_POINTS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MISCDATA JOB_POINTS packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::JobPoints), 0x05, "JobPoints miscdata type") && ok;
    ok      = expectEqualUInt(jobPointsPacketDataSize, 152, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(jobPointsPacketSize, 156, "packet size") && ok;
    ok      = expectEqualUInt(jobPointsTypeOffset, 4, "type offset") && ok;
    ok      = expectEqualUInt(jobPointsUnknown06Offset, 6, "unknown06 offset") && ok;
    ok      = expectEqualUInt(jobPointsAccessOffset, 8, "access offset") && ok;
    ok      = expectEqualUInt(jobPointsPaddingOffset, 9, "padding offset") && ok;
    ok      = expectEqualUInt(jobPointsJobsOffset, 12, "jobs offset") && ok;
    ok      = expectEqualUInt(jobPointsJob1Offset, 18, "jobs[1] offset") && ok;
    ok      = expectEqualUInt(jobPointsJob23Offset, 150, "jobs[23] offset") && ok;
    ok      = expectEqualUInt(jobPointsCapacityPointsOffset, 12, "capacityPoints offset") && ok;
    ok      = expectEqualUInt(jobPointsCurrentJpOffset, 14, "currentJp offset") && ok;
    ok      = expectEqualUInt(jobPointsTotalJpSpentOffset, 16, "totalJpSpent offset") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataJobPointsPacket::PacketData::JobPointData), 6, "JobPointData size") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataJobPointsPacket::PacketData::jobs), 144, "jobs size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = MiscDataJobPointsPacket::PacketData{};

    data.type                    = GP_SERV_COMMAND_MISCDATA_TYPE::JobPoints;
    data.unknown06               = jobPointsPacketDataSize;
    data.access                  = 1;
    data.jobs[1].capacityPoints  = 0x0102;
    data.jobs[1].currentJp       = 0x0304;
    data.jobs[1].totalJpSpent    = 0x0506;
    data.jobs[23].capacityPoints = 0x2122;
    data.jobs[23].currentJp      = 0x2324;
    data.jobs[23].totalJpSpent   = 0x2526;

    auto expected = std::array<uint8, jobPointsPacketDataSize>{};
    putLE16(expected, 0, 0x05);
    putLE16(expected, 2, jobPointsPacketDataSize);
    expected[4] = 0x01;
    putLE16(expected, 14, 0x0102);
    putLE16(expected, 16, 0x0304);
    putLE16(expected, 18, 0x0506);
    putLE16(expected, 146, 0x2122);
    putLE16(expected, 148, 0x2324);
    putLE16(expected, 150, 0x2526);

    return expectStructBytes(data, expected, "JOB_POINTS PacketData bytes");
}

auto testRuntimeShaping() -> bool
{
    auto facts        = miscdatajobpointshelpers::Facts{};
    facts.access      = true;
    facts.jobs[0]     = { 1, 2, 3 };
    facts.jobs[1]     = { 0x0102, 0x0304, 0x0506 };
    facts.jobs[23]    = { 0x2122, 0x2324, 0x2526 };
    const auto packet = miscdatajobpointshelpers::PlanFor(facts);

    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(packet.type), 0x05, "runtime type") && ok;
    ok      = expectEqualUInt(packet.unknown06, jobPointsPacketDataSize, "runtime size marker") && ok;
    ok      = expectEqualUInt(packet.access, 1, "runtime access") && ok;
    ok      = expectEqualUInt(packet.jobs[0].capacityPoints, 0, "runtime job zero remains unused") && ok;
    ok      = expectEqualUInt(packet.jobs[1].currentJp, 0x0304, "runtime job one") && ok;
    ok      = expectEqualUInt(packet.jobs[23].totalJpSpent, 0x2526, "runtime last job") && ok;
    return ok;
}

} // namespace

auto runS2CMiscDataJobPointsPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testRuntimeShaping() && ok;
    return ok;
}
