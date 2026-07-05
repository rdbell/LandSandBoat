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

#include "test_s2c_group_solicit_req_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/enums/party_kind.h"
#include "map/packets/s2c/0x0dc_group_solicit_req.h"

namespace
{

using GroupSolicitReqPacket = GP_SERV_COMMAND_GROUP_SOLICIT_REQ;

constexpr auto groupSolicitReqUniqueNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GroupSolicitReqPacket::PacketData, UniqueNo);
constexpr auto groupSolicitReqActIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(GroupSolicitReqPacket::PacketData, ActIndex);
constexpr auto groupSolicitReqAnonFlagOffset   = sizeof(GP_SERV_HEADER) + offsetof(GroupSolicitReqPacket::PacketData, AnonFlag);
constexpr auto groupSolicitReqKindOffset       = sizeof(GP_SERV_HEADER) + offsetof(GroupSolicitReqPacket::PacketData, Kind);
constexpr auto groupSolicitReqNameOffset       = sizeof(GP_SERV_HEADER) + offsetof(GroupSolicitReqPacket::PacketData, sName);
constexpr auto groupSolicitReqNameSize         = sizeof(GroupSolicitReqPacket::PacketData{}.sName);
constexpr auto groupSolicitReqRaceNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GroupSolicitReqPacket::PacketData, RaceNo);
constexpr auto groupSolicitReqPadding1EOffset  = sizeof(GP_SERV_HEADER) + offsetof(GroupSolicitReqPacket::PacketData, padding1E);
constexpr auto groupSolicitReqPadding1ESize    = sizeof(GroupSolicitReqPacket::PacketData{}.padding1E);
constexpr auto groupSolicitReqPacketDataSize   = sizeof(GroupSolicitReqPacket::PacketData);
constexpr auto groupSolicitReqPacketSize       = sizeof(GP_SERV_HEADER) + groupSolicitReqPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_SOLICIT_REQ packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_SOLICIT_REQ packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_GROUP_SOLICIT_REQ), 0x0DC, "GROUP_SOLICIT_REQ packet id") && ok;
    ok      = expectEqualUInt(groupSolicitReqNameSize, 16, "sName size") && ok;
    ok      = expectEqualUInt(groupSolicitReqPadding1ESize, 2, "padding1E size") && ok;
    ok      = expectEqualUInt(groupSolicitReqPacketDataSize, 28, "sizeof(GROUP_SOLICIT_REQ::PacketData)") && ok;
    ok      = expectEqualUInt(groupSolicitReqPacketSize, 32, "GROUP_SOLICIT_REQ packet size") && ok;
    ok      = expectEqualUInt(groupSolicitReqUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(groupSolicitReqActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(groupSolicitReqAnonFlagOffset, 10, "AnonFlag offset") && ok;
    ok      = expectEqualUInt(groupSolicitReqKindOffset, 11, "Kind offset") && ok;
    ok      = expectEqualUInt(groupSolicitReqNameOffset, 12, "sName offset") && ok;
    ok      = expectEqualUInt(groupSolicitReqRaceNoOffset, 28, "RaceNo offset") && ok;
    ok      = expectEqualUInt(groupSolicitReqPadding1EOffset, 30, "padding1E offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data          = GroupSolicitReqPacket::PacketData{};
    data.UniqueNo      = 0x11223344;
    data.ActIndex      = 0x5566;
    data.AnonFlag      = 0x77;
    data.Kind          = PartyKind::Alliance;
    data.sName[0]      = 0x88;
    data.sName[15]     = 0x99;
    data.RaceNo        = 0xAABB;
    data.padding1E[0]  = 0xCC;
    data.padding1E[1]  = 0xDD;

    auto expected = std::array<uint8, 28>{};
    expected[0]   = 0x44;
    expected[1]   = 0x33;
    expected[2]   = 0x22;
    expected[3]   = 0x11;
    expected[4]   = 0x66;
    expected[5]   = 0x55;
    expected[6]   = 0x77;
    expected[7]   = 0x05;
    expected[8]   = 0x88;
    expected[23]  = 0x99;
    expected[24]  = 0xBB;
    expected[25]  = 0xAA;
    expected[26]  = 0xCC;
    expected[27]  = 0xDD;

    return expectStructBytes(data, expected, "GROUP_SOLICIT_REQ PacketData bytes");
}

auto testConstructorBytes() -> bool
{
    auto packet = GroupSolicitReqPacket(0x11223344, 0x5566, "InviterName", PartyKind::Alliance);

    bool ok = true;
    ok      = expectEqualUInt(packet.ref<uint32>(groupSolicitReqUniqueNoOffset), 0x11223344, "constructor UniqueNo") && ok;
    ok      = expectEqualUInt(packet.ref<uint16>(groupSolicitReqActIndexOffset), 0x5566, "constructor ActIndex") && ok;
    ok      = expectEqualUInt(packet.ref<uint8>(groupSolicitReqAnonFlagOffset), 0, "constructor AnonFlag") && ok;
    ok      = expectEqualUInt(packet.ref<uint8>(groupSolicitReqKindOffset), static_cast<uint8>(PartyKind::Alliance), "constructor Kind") && ok;
    ok      = expectEqualUInt(packet.ref<uint8>(groupSolicitReqNameOffset), static_cast<uint8>('I'), "constructor sName first byte") && ok;
    ok      = expectEqualUInt(packet.ref<uint16>(groupSolicitReqRaceNoOffset), 0, "constructor RaceNo") && ok;
    return ok;
}

} // namespace

auto runS2CGroupSolicitReqPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testConstructorBytes() && ok;
    return ok;
}
