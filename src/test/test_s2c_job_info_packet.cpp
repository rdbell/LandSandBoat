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

#include "test_s2c_job_info_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x01b_job_info.h"

namespace
{

using JobInfoPacket = GP_SERV_COMMAND_JOB_INFO;

constexpr auto jobInfoDancerSize       = sizeof(GP_MYROOM_DANCER);
constexpr auto jobInfoPacketDataSize   = sizeof(JobInfoPacket::PacketData);
constexpr auto jobInfoPacketSize       = sizeof(GP_SERV_HEADER) + jobInfoPacketDataSize;
constexpr auto jobInfoDancerOffset     = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer);
constexpr auto jobInfoMonNoOffset      = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, mon_no);
constexpr auto jobInfoMJobOffset       = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, mjob_no);
constexpr auto jobInfoGetJobFlagOffset = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, get_job_flag);
constexpr auto jobInfoJobLevOffset     = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, job_lev);
constexpr auto jobInfoBPBaseOffset     = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, bp_base);
constexpr auto jobInfoBPAdjOffset      = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, bp_adj);
constexpr auto jobInfoHPMaxOffset      = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, hpmax);
constexpr auto jobInfoSJobFlagOffset   = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, sjobflg);
constexpr auto jobInfoJobLev2Offset    = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, dancer) + offsetof(GP_MYROOM_DANCER, job_lev2);
constexpr auto jobInfoEncumbranceOffset = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, encumbrance);
constexpr auto jobInfoMentorOffset      = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, can_thumbs_up_mentor);
constexpr auto jobInfoMasteryFlagsOffset = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, job_mastery_flags);
constexpr auto jobInfoMasteryLevelsOffset = sizeof(GP_SERV_HEADER) + offsetof(JobInfoPacket::PacketData, job_mastery_levels);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c JOB_INFO packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c JOB_INFO packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_JOB_INFO), 0x01B, "JOB_INFO packet id") && ok;
    ok      = expectEqualUInt(jobInfoDancerSize, 92, "sizeof(GP_MYROOM_DANCER)") && ok;
    ok      = expectEqualUInt(jobInfoPacketDataSize, 128, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(jobInfoPacketSize, 132, "packet size") && ok;
    ok      = expectEqualUInt(jobInfoDancerOffset, 4, "dancer offset") && ok;
    ok      = expectEqualUInt(jobInfoMonNoOffset, 4, "dancer.mon_no offset") && ok;
    ok      = expectEqualUInt(jobInfoMJobOffset, 8, "dancer.mjob_no offset") && ok;
    ok      = expectEqualUInt(jobInfoGetJobFlagOffset, 12, "dancer.get_job_flag offset") && ok;
    ok      = expectEqualUInt(jobInfoJobLevOffset, 16, "dancer.job_lev offset") && ok;
    ok      = expectEqualUInt(jobInfoBPBaseOffset, 32, "dancer.bp_base offset") && ok;
    ok      = expectEqualUInt(jobInfoBPAdjOffset, 46, "dancer.bp_adj offset") && ok;
    ok      = expectEqualUInt(jobInfoHPMaxOffset, 60, "dancer.hpmax offset") && ok;
    ok      = expectEqualUInt(jobInfoSJobFlagOffset, 68, "dancer.sjobflg offset") && ok;
    ok      = expectEqualUInt(jobInfoJobLev2Offset, 72, "dancer.job_lev2 offset") && ok;
    ok      = expectEqualUInt(jobInfoEncumbranceOffset, 96, "encumbrance offset") && ok;
    ok      = expectEqualUInt(jobInfoMentorOffset, 100, "can_thumbs_up_mentor offset") && ok;
    ok      = expectEqualUInt(jobInfoMasteryFlagsOffset, 104, "job_mastery_flags offset") && ok;
    ok      = expectEqualUInt(jobInfoMasteryLevelsOffset, 108, "job_mastery_levels offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER::job_lev), 16, "job_lev size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER::bp_base), 14, "bp_base size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER::bp_adj), 14, "bp_adj size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER::unknown41), 3, "unknown41 size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER::job_lev2), 24, "job_lev2 size") && ok;
    ok      = expectEqualUInt(sizeof(JobInfoPacket::PacketData::job_mastery_levels), 24, "job_mastery_levels size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = JobInfoPacket::PacketData{};

    data.dancer.mon_no       = 0x0102;
    data.dancer.face_no      = 0x0304;
    data.dancer.mjob_no      = static_cast<JOBTYPE>(0x05);
    data.dancer.hair_no      = 0x06;
    data.dancer.size         = 0x07;
    data.dancer.sjob_no      = static_cast<JOBTYPE>(0x08);
    data.dancer.get_job_flag = 0x11121314;
    for (std::size_t i = 0; i < 16; ++i)
    {
        data.dancer.job_lev[i] = static_cast<int8_t>(0x21 + i);
    }
    for (std::size_t i = 0; i < 7; ++i)
    {
        data.dancer.bp_base[i] = static_cast<uint16_t>(0x3132 + (i * 0x0202));
        data.dancer.bp_adj[i]  = static_cast<int16_t>(0x5152 + (i * 0x0202));
    }
    data.dancer.hpmax   = 0x61626364;
    data.dancer.mpmax   = 0x65666768;
    data.dancer.sjobflg = 0x69;
    data.dancer.unknown41[0] = 0x6A;
    data.dancer.unknown41[1] = 0x6B;
    data.dancer.unknown41[2] = 0x6C;
    for (std::size_t i = 0; i < 24; ++i)
    {
        data.dancer.job_lev2[i] = static_cast<uint8>(0x71 + i);
    }

    data.encumbrance          = 0x21222324;
    data.can_thumbs_up_mentor = 0x25;
    data.mentor_rank          = 0x26;
    data.mastery_rank         = 0x27;
    data.padding67            = 0x28;
    data.job_mastery_flags    = 0x292A2B2C;
    for (std::size_t i = 0; i < 24; ++i)
    {
        data.job_mastery_levels[i] = static_cast<uint8>(0x31 + i);
    }

    auto expected = std::array<uint8, jobInfoPacketDataSize>{};
    putLE16(expected, 0, 0x0102);
    putLE16(expected, 2, 0x0304);
    expected[4] = 0x05;
    expected[5] = 0x06;
    expected[6] = 0x07;
    expected[7] = 0x08;
    putLE32(expected, 8, 0x11121314);
    for (std::size_t i = 0; i < 16; ++i)
    {
        expected[12 + i] = static_cast<uint8>(0x21 + i);
    }
    for (std::size_t i = 0; i < 7; ++i)
    {
        putLE16(expected, 28 + (i * 2), static_cast<uint16_t>(0x3132 + (i * 0x0202)));
        putLE16(expected, 42 + (i * 2), static_cast<uint16_t>(0x5152 + (i * 0x0202)));
    }
    putLE32(expected, 56, 0x61626364);
    putLE32(expected, 60, 0x65666768);
    expected[64] = 0x69;
    expected[65] = 0x6A;
    expected[66] = 0x6B;
    expected[67] = 0x6C;
    for (std::size_t i = 0; i < 24; ++i)
    {
        expected[68 + i] = static_cast<uint8>(0x71 + i);
    }
    putLE32(expected, 92, 0x21222324);
    expected[96] = 0x25;
    expected[97] = 0x26;
    expected[98] = 0x27;
    expected[99] = 0x28;
    putLE32(expected, 100, 0x292A2B2C);
    for (std::size_t i = 0; i < 24; ++i)
    {
        expected[104 + i] = static_cast<uint8>(0x31 + i);
    }

    return expectStructBytes(data, expected, "JOB_INFO PacketData bytes");
}

} // namespace

auto runS2CJobInfoPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
