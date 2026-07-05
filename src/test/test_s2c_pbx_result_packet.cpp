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

#include "test_s2c_pbx_result_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/c2s/0x04d_pbx.h"
#include "map/packets/s2c/0x04b_pbx_result.h"

namespace
{

constexpr auto pbxCommandOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, Command);
constexpr auto pbxBoxNoOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, BoxNo);
constexpr auto pbxPostWorkNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, PostWorkNo);
constexpr auto pbxItemWorkNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, ItemWorkNo);
constexpr auto pbxItemStacksOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, ItemStacks);
constexpr auto pbxResultOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, Result);
constexpr auto pbxResParam1Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, ResParam1);
constexpr auto pbxResParam2Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, ResParam2);
constexpr auto pbxResParam3Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, ResParam3);
constexpr auto pbxRepresentOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PBX_RESULT::PacketData, Represent);
constexpr auto pbxPacketDataSize     = sizeof(GP_SERV_COMMAND_PBX_RESULT::PacketData);
constexpr auto pbxFullPacketSize     = sizeof(GP_SERV_HEADER) + pbxPacketDataSize;
constexpr auto pbxStatelessPacketSize = 20U;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c PBX_RESULT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c PBX_RESULT packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
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

auto expectRepeatedByte(CBasicPacket& packet, std::size_t offset, std::size_t count, uint8 expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = 0; i < count; ++i)
    {
        if (data[offset + i] != expected)
        {
            std::cerr << "s2c PBX_RESULT packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[offset + i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_POST_BOX_STATE), 72, "sizeof(GP_POST_BOX_STATE)") && ok;
    ok      = expectEqualUInt(pbxPacketDataSize, 84, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(pbxFullPacketSize, 88, "full packet size") && ok;
    ok      = expectEqualUInt(pbxCommandOffset, 4, "Command offset") && ok;
    ok      = expectEqualUInt(pbxBoxNoOffset, 5, "BoxNo offset") && ok;
    ok      = expectEqualUInt(pbxPostWorkNoOffset, 6, "PostWorkNo offset") && ok;
    ok      = expectEqualUInt(pbxItemWorkNoOffset, 7, "ItemWorkNo offset") && ok;
    ok      = expectEqualUInt(pbxItemStacksOffset, 8, "ItemStacks offset") && ok;
    ok      = expectEqualUInt(pbxResultOffset, 12, "Result offset") && ok;
    ok      = expectEqualUInt(pbxResParam1Offset, 13, "ResParam1 offset") && ok;
    ok      = expectEqualUInt(pbxResParam2Offset, 14, "ResParam2 offset") && ok;
    ok      = expectEqualUInt(pbxResParam3Offset, 15, "ResParam3 offset") && ok;
    ok      = expectEqualUInt(pbxRepresentOffset, 16, "Represent offset") && ok;
    return ok;
}

auto testCheckIncomingConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_PBX_RESULT(GP_CLI_COMMAND_PBX_COMMAND::Check, GP_CLI_COMMAND_PBX_BOXNO::Incoming, 7, 2);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x04B, "check incoming type") && ok;
    ok      = expectEqualUInt(packet.getSize(), pbxStatelessPacketSize, "check incoming size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x4B, 0x0A, 0xEF, 0xBE }, "check incoming header") && ok;
    ok      = expectBytes(packet, pbxCommandOffset, std::array<uint8, 12>{ 0x05, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0xFF, 0x07, 0xFF }, "check incoming scalar fields") && ok;
    ok      = expectRepeatedByte(packet, pbxRepresentOffset, 4, 0, "check incoming represent") && ok;
    ok      = expectRepeatedByte(packet, pbxStatelessPacketSize, PACKET_SIZE - pbxStatelessPacketSize, 0, "check incoming tail") && ok;
    return ok;
}

auto testCheckOutgoingConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_PBX_RESULT(GP_CLI_COMMAND_PBX_COMMAND::Check, GP_CLI_COMMAND_PBX_BOXNO::Outgoing, 9, 3);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), pbxStatelessPacketSize, "check outgoing size") && ok;
    ok      = expectBytes(packet, pbxCommandOffset, std::array<uint8, 12>{ 0x05, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0xFF, 0x09 }, "check outgoing scalar fields") && ok;
    return ok;
}

auto testQueryConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_PBX_RESULT(GP_CLI_COMMAND_PBX_COMMAND::Query, GP_CLI_COMMAND_PBX_BOXNO::None, 4, 1);

    bool ok = true;
    ok      = expectBytes(packet, pbxCommandOffset, std::array<uint8, 12>{ 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x04, 0xFF, 0xFF }, "query scalar fields") && ok;
    return ok;
}

auto testOtherActionLeavesCountFieldsUnused() -> bool
{
    auto packet = GP_SERV_COMMAND_PBX_RESULT(GP_CLI_COMMAND_PBX_COMMAND::DeliOpen, GP_CLI_COMMAND_PBX_BOXNO::None, 6, 1);

    bool ok = true;
    ok      = expectBytes(packet, pbxCommandOffset, std::array<uint8, 12>{ 0x0D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0xFF, 0xFF, 0xFF }, "other action scalar fields") && ok;
    return ok;
}

} // namespace

auto runS2CPBXResultPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testCheckIncomingConstructor() && ok;
    ok      = testCheckOutgoingConstructor() && ok;
    ok      = testQueryConstructor() && ok;
    ok      = testOtherActionLeavesCountFieldsUnused() && ok;
    return ok;
}
