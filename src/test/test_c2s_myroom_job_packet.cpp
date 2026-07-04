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

#include "test_c2s_myroom_job_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/entities/battle_entity.h"
#include "map/packets/c2s/0x100_myroom_job.h"

namespace
{

using MyRoomJobBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_JOB)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom job packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s myroom job packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s myroom job packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom job packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom job packet self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << " expected";
        for (const auto value : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto expectValid(const PacketValidationResult& result, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectTrue(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), "", label + " error string") && ok;
    return ok;
}

auto expectInvalidError(const PacketValidationResult& result, const std::string& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), expected, label + " error string") && ok;
    return ok;
}

template <typename Packet>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, sizeof(Packet)>
{
    auto bytes = std::array<std::uint8_t, sizeof(Packet)>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto validateMyRoomJobPure(std::uint8_t mainJobIndex, std::uint8_t supportJobIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    if (mainJobIndex)
    {
        validator.range("MainJobIndex", mainJobIndex, 0x01, MAX_JOBTYPE - 1);
    }
    if (supportJobIndex)
    {
        validator.range("SupportJobIndex", supportJobIndex, 0x00, MAX_JOBTYPE - 1);
    }
    return validator;
}

auto makeMyRoomJobPacket() -> GP_CLI_COMMAND_MYROOM_JOB
{
    auto packet             = GP_CLI_COMMAND_MYROOM_JOB{};
    packet.header.id       = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_JOB);
    packet.header.size     = sizeof(GP_CLI_COMMAND_MYROOM_JOB) / 4U;
    packet.header.sync     = 0xBEEF;
    packet.MainJobIndex    = JOB_BLU;
    packet.SupportJobIndex = JOB_PUP;
    packet.padding00       = 0xAABB;
    return packet;
}

auto testMyRoomJobLayoutAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_MYROOM_JOB::name, "GP_CLI_COMMAND_MYROOM_JOB", "MYROOM_JOB name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_JOB::packetId), 0x100, "MYROOM_JOB packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_JOB), 8, "MYROOM_JOB sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_JOB, MainJobIndex), 4, "MYROOM_JOB MainJobIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_JOB, SupportJobIndex), 5, "MYROOM_JOB SupportJobIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_JOB, padding00), 6, "MYROOM_JOB padding00 offset") && ok;
    ok      = expectEqualInt(JOB_NON, 0, "JOB_NON") && ok;
    ok      = expectEqualInt(JOB_WAR, 1, "JOB_WAR") && ok;
    ok      = expectEqualInt(JOB_MON, 23, "JOB_MON") && ok;
    ok      = expectEqualInt(MAX_JOBTYPE, 24, "MAX_JOBTYPE") && ok;
    return ok;
}

auto testMyRoomJobEncodedBytesAndPayload() -> bool
{
    return expectBytes(encodedPacketBytes(makeMyRoomJobPacket()), MyRoomJobBytes{ 0x00, 0x05, 0xEF, 0xBE, 0x10, 0x12, 0xBB, 0xAA }, "MYROOM_JOB encoded packet");
}

auto testMyRoomJobValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateMyRoomJobPure(0, 0), "MYROOM_JOB no changes validation") && ok;
    ok      = expectValid(validateMyRoomJobPure(1, 1), "MYROOM_JOB min job validation") && ok;
    ok      = expectValid(validateMyRoomJobPure(MAX_JOBTYPE - 1, MAX_JOBTYPE - 1), "MYROOM_JOB max job validation") && ok;
    ok      = expectInvalidError(validateMyRoomJobPure(MAX_JOBTYPE, 0), "MainJobIndex out of range: 24 not in [1, 23]", "MYROOM_JOB invalid main validation") && ok;
    ok      = expectInvalidError(validateMyRoomJobPure(0, MAX_JOBTYPE), "SupportJobIndex out of range: 24 not in [0, 23]", "MYROOM_JOB invalid support validation") && ok;
    ok      = expectInvalidError(validateMyRoomJobPure(MAX_JOBTYPE, MAX_JOBTYPE), "MainJobIndex out of range: 24 not in [1, 23]", "MYROOM_JOB validation order") && ok;
    return ok;
}

} // namespace

auto runC2SMyRoomJobPacketSelfTests() -> bool
{
    return testMyRoomJobLayoutAndMetadata() &&
           testMyRoomJobEncodedBytesAndPayload() &&
           testMyRoomJobValidation();
}
