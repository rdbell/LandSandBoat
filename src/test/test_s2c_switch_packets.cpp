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

#include "test_s2c_switch_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/nominate_manager.h"
#include "map/packets/s2c/0x078_switch_start.h"
#include "map/packets/s2c/0x079_switch_proc.h"

namespace
{

constexpr auto switchStartUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_START::PacketData, UniqueNo);
constexpr auto switchStartAllNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_START::PacketData, AllNum);
constexpr auto switchStartActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_START::PacketData, ActIndex);
constexpr auto switchStartNameOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_START::PacketData, sName);
constexpr auto switchStartKindOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_START::PacketData, Kind);
constexpr auto switchStartStrOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_START::PacketData, Str);
constexpr auto switchStartPacketDataSize = sizeof(GP_SERV_COMMAND_SWITCH_START::PacketData);
constexpr auto switchStartFullPacketSize = sizeof(GP_SERV_HEADER) + switchStartPacketDataSize;

constexpr auto switchProcAllNumOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_PROC::PacketData, AllNum);
constexpr auto switchProcVoteNumTblOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_PROC::PacketData, VoteNumTbl);
constexpr auto switchProcKindOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_PROC::PacketData, Kind);
constexpr auto switchProcStateOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_PROC::PacketData, State);
constexpr auto switchProcQuestionNumOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_PROC::PacketData, QuestionNum);
constexpr auto switchProcNameOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_PROC::PacketData, sPropName);
constexpr auto switchProcStrOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SWITCH_PROC::PacketData, Str);
constexpr auto switchProcPacketDataSize    = sizeof(GP_SERV_COMMAND_SWITCH_PROC::PacketData);
constexpr auto switchProcFullPacketSize    = sizeof(GP_SERV_HEADER) + switchProcPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c SWITCH packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c SWITCH packet self-test failed: " << label << " got";
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
            std::cerr << "s2c SWITCH packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[offset + i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    return expectRepeatedByte(packet, offset, end - offset, 0, label);
}

auto makeProposal() -> NominateProposal
{
    NominateProposal proposal{};
    proposal.proposerId       = 0x11223344;
    proposal.proposerName     = "ProposalSenderXYZ";
    proposal.proposerActIndex = 0x7788;
    proposal.kind             = GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Shout;
    proposal.allNum           = 0x3344;
    proposal.options          = { "yes", "no" };
    proposal.voteTbl          = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    return proposal;
}

auto testSwitchStartLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(switchStartPacketDataSize, 252, "SWITCH_START sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(switchStartFullPacketSize, 256, "SWITCH_START full packet size") && ok;
    ok      = expectEqualUInt(switchStartUniqueNoOffset, 4, "SWITCH_START UniqueNo offset") && ok;
    ok      = expectEqualUInt(switchStartAllNumOffset, 8, "SWITCH_START AllNum offset") && ok;
    ok      = expectEqualUInt(switchStartActIndexOffset, 12, "SWITCH_START ActIndex offset") && ok;
    ok      = expectEqualUInt(switchStartNameOffset, 14, "SWITCH_START sName offset") && ok;
    ok      = expectEqualUInt(switchStartKindOffset, 29, "SWITCH_START Kind offset") && ok;
    ok      = expectEqualUInt(switchStartStrOffset, 30, "SWITCH_START Str offset") && ok;
    return ok;
}

auto testSwitchStartConstructor() -> bool
{
    auto proposal = makeProposal();
    auto packet   = GP_SERV_COMMAND_SWITCH_START(proposal, "Pick one");
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x078, "SWITCH_START type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 40, "SWITCH_START short size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x78, 0x14, 0xEF, 0xBE }, "SWITCH_START header") && ok;
    ok      = expectBytes(packet, switchStartUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "SWITCH_START UniqueNo") && ok;
    ok      = expectBytes(packet, switchStartAllNumOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x00, 0x00 }, "SWITCH_START AllNum") && ok;
    ok      = expectBytes(packet, switchStartActIndexOffset, std::array<uint8, 2>{ 0x88, 0x77 }, "SWITCH_START ActIndex") && ok;
    ok      = expectBytes(packet, switchStartNameOffset, std::array<uint8, 15>{ 'P', 'r', 'o', 'p', 'o', 's', 'a', 'l', 'S', 'e', 'n', 'd', 'e', 'r', 'X' }, "SWITCH_START name") && ok;
    ok      = expectBytes(packet, switchStartKindOffset, std::array<uint8, 1>{ 0x06 }, "SWITCH_START Kind") && ok;
    ok      = expectBytes(packet, switchStartStrOffset, std::array<uint8, 8>{ 'P', 'i', 'c', 'k', ' ', 'o', 'n', 'e' }, "SWITCH_START Str") && ok;
    ok      = expectZeroRange(packet, switchStartStrOffset + 8, PACKET_SIZE, "SWITCH_START padding and tail") && ok;
    return ok;
}

auto testSwitchStartClamp() -> bool
{
    auto proposal = makeProposal();
    auto packet   = GP_SERV_COMMAND_SWITCH_START(proposal, std::string(260, 'Q'));

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 256, "SWITCH_START clamped size") && ok;
    ok      = expectRepeatedByte(packet, switchStartStrOffset, 223, 'Q', "SWITCH_START clamped Str") && ok;
    ok      = expectZeroRange(packet, switchStartStrOffset + 223, PACKET_SIZE, "SWITCH_START clamped terminator and tail") && ok;
    return ok;
}

auto testSwitchProcLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(switchProcPacketDataSize, 296, "SWITCH_PROC sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(switchProcFullPacketSize, 300, "SWITCH_PROC full packet size") && ok;
    ok      = expectEqualUInt(switchProcAllNumOffset, 4, "SWITCH_PROC AllNum offset") && ok;
    ok      = expectEqualUInt(switchProcVoteNumTblOffset, 8, "SWITCH_PROC VoteNumTbl offset") && ok;
    ok      = expectEqualUInt(switchProcKindOffset, 26, "SWITCH_PROC Kind offset") && ok;
    ok      = expectEqualUInt(switchProcStateOffset, 27, "SWITCH_PROC State offset") && ok;
    ok      = expectEqualUInt(switchProcQuestionNumOffset, 28, "SWITCH_PROC QuestionNum offset") && ok;
    ok      = expectEqualUInt(switchProcNameOffset, 29, "SWITCH_PROC sPropName offset") && ok;
    ok      = expectEqualUInt(switchProcStrOffset, 44, "SWITCH_PROC Str offset") && ok;
    return ok;
}

auto testSwitchProcConstructor() -> bool
{
    auto proposal = makeProposal();
    auto packet   = GP_SERV_COMMAND_SWITCH_PROC(proposal, GP_SERV_COMMAND_SWITCH_PROC_STATE::Closed, "Tallies");
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x079, "SWITCH_PROC type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 56, "SWITCH_PROC short size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x79, 0x1C, 0xEF, 0xBE }, "SWITCH_PROC header") && ok;
    ok      = expectBytes(packet, switchProcAllNumOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x00, 0x00 }, "SWITCH_PROC AllNum") && ok;
    ok      = expectBytes(packet, switchProcVoteNumTblOffset, std::array<uint8, 18>{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0 }, "SWITCH_PROC VoteNumTbl") && ok;
    ok      = expectBytes(packet, switchProcKindOffset, std::array<uint8, 1>{ 0x06 }, "SWITCH_PROC Kind") && ok;
    ok      = expectBytes(packet, switchProcStateOffset, std::array<uint8, 1>{ 0x02 }, "SWITCH_PROC State") && ok;
    ok      = expectBytes(packet, switchProcQuestionNumOffset, std::array<uint8, 1>{ 0x03 }, "SWITCH_PROC QuestionNum") && ok;
    ok      = expectBytes(packet, switchProcNameOffset, std::array<uint8, 15>{ 'P', 'r', 'o', 'p', 'o', 's', 'a', 'l', 'S', 'e', 'n', 'd', 'e', 'r', 'X' }, "SWITCH_PROC name") && ok;
    ok      = expectBytes(packet, switchProcStrOffset, std::array<uint8, 7>{ 'T', 'a', 'l', 'l', 'i', 'e', 's' }, "SWITCH_PROC Str") && ok;
    ok      = expectZeroRange(packet, switchProcStrOffset + 7, PACKET_SIZE, "SWITCH_PROC padding and tail") && ok;
    return ok;
}

auto testSwitchProcClamp() -> bool
{
    auto proposal = makeProposal();
    auto packet   = GP_SERV_COMMAND_SWITCH_PROC(proposal, GP_SERV_COMMAND_SWITCH_PROC_STATE::Active, std::string(300, 'R'));

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 304, "SWITCH_PROC clamped size") && ok;
    ok      = expectRepeatedByte(packet, switchProcStrOffset, 255, 'R', "SWITCH_PROC clamped Str") && ok;
    ok      = expectZeroRange(packet, switchProcStrOffset + 255, PACKET_SIZE, "SWITCH_PROC clamped terminator and tail") && ok;
    return ok;
}

} // namespace

auto runS2CSwitchPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testSwitchStartLayout() && ok;
    ok      = testSwitchStartConstructor() && ok;
    ok      = testSwitchStartClamp() && ok;
    ok      = testSwitchProcLayout() && ok;
    ok      = testSwitchProcConstructor() && ok;
    ok      = testSwitchProcClamp() && ok;
    return ok;
}
