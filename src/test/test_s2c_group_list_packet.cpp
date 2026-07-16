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

#include "test_s2c_group_list_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0dd_group_list.h"

namespace
{

using GroupListPacket = GP_SERV_COMMAND_GROUP_LIST;

constexpr auto groupListUniqueNoOffset       = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, UniqueNo);
constexpr auto groupListHpOffset             = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, Hp);
constexpr auto groupListMpOffset             = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, Mp);
constexpr auto groupListTpOffset             = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, Tp);
constexpr auto groupListGAttrOffset          = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, GAttr);
constexpr auto groupListActIndexOffset       = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, ActIndex);
constexpr auto groupListMemberNumberOffset   = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, MemberNumber);
constexpr auto groupListMoghouseFlgOffset    = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, MoghouseFlg);
constexpr auto groupListKindOffset           = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, Kind);
constexpr auto groupListHppOffset            = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, Hpp);
constexpr auto groupListMppOffset            = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, Mpp);
constexpr auto groupListPadding1FOffset      = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, padding1F);
constexpr auto groupListZoneNoOffset         = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, ZoneNo);
constexpr auto groupListMJobNoOffset         = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, mjob_no);
constexpr auto groupListMJobLvOffset         = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, mjob_lv);
constexpr auto groupListSJobNoOffset         = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, sjob_no);
constexpr auto groupListSJobLvOffset         = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, sjob_lv);
constexpr auto groupListMasterJobLvOffset    = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, masterjob_lv);
constexpr auto groupListMasterJobFlagsOffset = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, masterjob_flags);
constexpr auto groupListNameOffset           = sizeof(GP_SERV_HEADER) + offsetof(GroupListPacket::PacketData, Name);
constexpr auto groupListNameSize             = sizeof(GroupListPacket::PacketData::Name);
constexpr auto groupListPacketDataSize       = sizeof(GroupListPacket::PacketData);
constexpr auto groupListMaxStructPacketSize  = sizeof(GP_SERV_HEADER) + groupListPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectedPacketSize(std::size_t nameSize) -> std::size_t
{
    const auto clampedNameSize = std::min(nameSize, static_cast<std::size_t>(groupListNameSize));
    return sizeof(GP_SERV_HEADER) + groupListPacketDataSize - groupListNameSize + roundUpToNearestFour(static_cast<uint32>(clampedNameSize)) + 4;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_LIST packet self-test failed: " << label << " got";
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

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < end; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c GROUP_LIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_GROUP_LIST), 0x0DD, "GROUP_LIST packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_GROUP_ATTR), 4, "sizeof(GP_GROUP_ATTR)") && ok;
    ok      = expectEqualUInt(groupListNameSize, 16, "Name size") && ok;
    ok      = expectEqualUInt(groupListPacketDataSize, 52, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(groupListMaxStructPacketSize, 56, "header + PacketData size") && ok;

    ok = expectEqualUInt(groupListUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok = expectEqualUInt(groupListHpOffset, 8, "Hp offset") && ok;
    ok = expectEqualUInt(groupListMpOffset, 12, "Mp offset") && ok;
    ok = expectEqualUInt(groupListTpOffset, 16, "Tp offset") && ok;
    ok = expectEqualUInt(groupListGAttrOffset, 20, "GAttr offset") && ok;
    ok = expectEqualUInt(groupListActIndexOffset, 24, "ActIndex offset") && ok;
    ok = expectEqualUInt(groupListMemberNumberOffset, 26, "MemberNumber offset") && ok;
    ok = expectEqualUInt(groupListMoghouseFlgOffset, 27, "MoghouseFlg offset") && ok;
    ok = expectEqualUInt(groupListKindOffset, 28, "Kind offset") && ok;
    ok = expectEqualUInt(groupListHppOffset, 29, "Hpp offset") && ok;
    ok = expectEqualUInt(groupListMppOffset, 30, "Mpp offset") && ok;
    ok = expectEqualUInt(groupListPadding1FOffset, 31, "padding1F offset") && ok;
    ok = expectEqualUInt(groupListZoneNoOffset, 32, "ZoneNo offset") && ok;
    ok = expectEqualUInt(groupListMJobNoOffset, 34, "mjob_no offset") && ok;
    ok = expectEqualUInt(groupListMJobLvOffset, 35, "mjob_lv offset") && ok;
    ok = expectEqualUInt(groupListSJobNoOffset, 36, "sjob_no offset") && ok;
    ok = expectEqualUInt(groupListSJobLvOffset, 37, "sjob_lv offset") && ok;
    ok = expectEqualUInt(groupListMasterJobLvOffset, 38, "masterjob_lv offset") && ok;
    ok = expectEqualUInt(groupListMasterJobFlagsOffset, 39, "masterjob_flags offset") && ok;
    ok = expectEqualUInt(groupListNameOffset, 40, "Name offset") && ok;
    return ok;
}

auto testConstructorBytes() -> bool
{
    auto packet = GroupListPacket(0x11223344, "PartyPal", 0x01FE, 0x07, 0x2468);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 52>{
        0xDD,
        0x1A,
        0xEF,
        0xBE,
        0x44,
        0x33,
        0x22,
        0x11,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xFE,
        0x01,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x68,
        0x24,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x50,
        0x61,
        0x72,
        0x74,
        0x79,
        0x50,
        0x61,
        0x6C,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0DD, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expectedPacketSize(8), "8-byte name packet size") && ok;
    ok      = expectBytes(packet, 0, expected, "encoded packet bytes") && ok;
    ok      = expectZeroRange(packet, expected.size(), PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testNameSizing() -> bool
{
    auto oneByteName = GroupListPacket(0x01020304, "A", 0, 0x09, 0x0506);
    auto longName    = GroupListPacket(0x01020304, "abcdefghijklmnopQ", 0, 0x09, 0x0506);

    const auto expectedLongName = std::array<uint8, 16>{
        0x61,
        0x62,
        0x63,
        0x64,
        0x65,
        0x66,
        0x67,
        0x68,
        0x69,
        0x6A,
        0x6B,
        0x6C,
        0x6D,
        0x6E,
        0x6F,
        0x70,
    };

    bool ok = true;
    ok      = expectEqualUInt(oneByteName.getSize(), expectedPacketSize(1), "1-byte name packet size") && ok;
    ok      = expectEqualUInt(longName.getSize(), expectedPacketSize(17), "truncated long-name packet size") && ok;
    ok      = expectBytes(longName, groupListNameOffset, expectedLongName, "truncated name bytes") && ok;
    ok      = expectZeroRange(longName, groupListNameOffset + expectedLongName.size(), longName.getSize(), "long-name padding") && ok;
    return ok;
}

auto testRuntimePlans() -> bool
{
    using namespace grouplisthelpers;
    const auto common = CommonFacts{ .uniqueNo = 0x11223344, .hp = 1000, .mp = 200, .tp = 3000, .actIndex = 0x1234, .hpp = 75, .mpp = 50, .mjobNo = 6, .mjobLv = 99, .sjobNo = 3, .sjobLv = 49, .name = "Member" };
    bool       ok     = true;

    auto character = CharacterFacts{ .common = common, .memberFlags = 0x01FE, .memberNumber = 2, .entityZone = 100, .requestedZone = 100 };
    auto plan      = CharacterPlanFor(character);
    ok             = expectEqualUInt(plan.data.Hp, 1000, "same-zone character HP") && ok;
    ok             = expectEqualUInt(plan.data.MemberNumber, 2, "same-zone character member number") && ok;
    ok             = expectEqualUInt(plan.data.mjob_no, 6, "same-zone character main job") && ok;

    character.anonymous = true;
    plan                = CharacterPlanFor(character);
    ok                  = expectEqualUInt(plan.data.mjob_no, 0, "anonymous character hides main job") && ok;
    ok                  = expectEqualUInt(plan.data.sjob_no, 0, "anonymous character hides support job") && ok;

    character.anonymous     = false;
    character.requestedZone = 101;
    plan                    = CharacterPlanFor(character);
    ok                      = expectEqualUInt(plan.data.ZoneNo, 100, "different-zone character zone") && ok;
    ok                      = expectEqualUInt(plan.data.Hp, 0, "different-zone character hides HP") && ok;
    ok                      = expectEqualUInt(plan.data.MemberNumber, 0, "different-zone character hides member number") && ok;

    auto trustPlan = TrustPlanFor({ .common = common, .memberNumber = 3, .packetName = "Trust" });
    ok             = expectEqualUInt(trustPlan.data.Hp, 1000, "trust HP") && ok;
    ok             = expectEqualUInt(trustPlan.data.MemberNumber, 3, "trust member number") && ok;
    ok             = expectEqualUInt(trustPlan.data.mjob_no, 6, "trust main job") && ok;
    ok             = expectEqualUInt(trustPlan.nameSize, 6, "trust uses entity name length for packet size") && ok;
    ok             = expectEqualUInt(trustPlan.data.Name[0], 'T', "trust uses packet name bytes") && ok;
    return ok;
}

} // namespace

auto runS2CGroupListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructorBytes() && ok;
    ok      = testNameSizing() && ok;
    ok      = testRuntimePlans() && ok;
    return ok;
}
