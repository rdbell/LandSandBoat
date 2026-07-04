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

#include "test_s2c_group_list2_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0e2_group_list2.h"

namespace
{

constexpr auto groupList2UniqueNoOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, UniqueNo);
constexpr auto groupList2HpOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, Hp);
constexpr auto groupList2MpOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, Mp);
constexpr auto groupList2TpOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, Tp);
constexpr auto groupList2GAttrOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, GAttr);
constexpr auto groupList2ActIndexOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, ActIndex);
constexpr auto groupList2MemberNumberOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, MemberNumber);
constexpr auto groupList2MoghouseFlgOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, MoghouseFlg);
constexpr auto groupList2KindOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, Kind);
constexpr auto groupList2HppOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, Hpp);
constexpr auto groupList2MppOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, Mpp);
constexpr auto groupList2Padding1FOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, padding1F);
constexpr auto groupList2ZoneNoOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, ZoneNo);
constexpr auto groupList2MJobNoOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, mjob_no);
constexpr auto groupList2MJobLvOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, mjob_lv);
constexpr auto groupList2SJobNoOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, sjob_no);
constexpr auto groupList2SJobLvOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, sjob_lv);
constexpr auto groupList2MasterJobLvOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, masterjob_lv);
constexpr auto groupList2MasterJobFlagsOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, masterjob_flags);
constexpr auto groupList2NameOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GROUP_LIST2::PacketData, Name);
constexpr auto groupList2PacketSize           = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_GROUP_LIST2::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_LIST2 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_LIST2 packet self-test failed: " << label << " got";
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
            std::cerr << "s2c GROUP_LIST2 packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_GROUP_ATTR), 4, "sizeof(GP_GROUP_ATTR)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_GROUP_LIST2::PacketData), 52, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(groupList2PacketSize, 56, "packet size") && ok;

    ok = expectEqualUInt(groupList2UniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok = expectEqualUInt(groupList2HpOffset, 8, "Hp offset") && ok;
    ok = expectEqualUInt(groupList2MpOffset, 12, "Mp offset") && ok;
    ok = expectEqualUInt(groupList2TpOffset, 16, "Tp offset") && ok;
    ok = expectEqualUInt(groupList2GAttrOffset, 20, "GAttr offset") && ok;
    ok = expectEqualUInt(groupList2ActIndexOffset, 24, "ActIndex offset") && ok;
    ok = expectEqualUInt(groupList2MemberNumberOffset, 26, "MemberNumber offset") && ok;
    ok = expectEqualUInt(groupList2MoghouseFlgOffset, 27, "MoghouseFlg offset") && ok;
    ok = expectEqualUInt(groupList2KindOffset, 28, "Kind offset") && ok;
    ok = expectEqualUInt(groupList2HppOffset, 29, "Hpp offset") && ok;
    ok = expectEqualUInt(groupList2MppOffset, 30, "Mpp offset") && ok;
    ok = expectEqualUInt(groupList2Padding1FOffset, 31, "padding1F offset") && ok;
    ok = expectEqualUInt(groupList2ZoneNoOffset, 32, "ZoneNo offset") && ok;
    ok = expectEqualUInt(groupList2MJobNoOffset, 34, "mjob_no offset") && ok;
    ok = expectEqualUInt(groupList2MJobLvOffset, 35, "mjob_lv offset") && ok;
    ok = expectEqualUInt(groupList2SJobNoOffset, 36, "sjob_no offset") && ok;
    ok = expectEqualUInt(groupList2SJobLvOffset, 37, "sjob_lv offset") && ok;
    ok = expectEqualUInt(groupList2MasterJobLvOffset, 38, "masterjob_lv offset") && ok;
    ok = expectEqualUInt(groupList2MasterJobFlagsOffset, 39, "masterjob_flags offset") && ok;
    ok = expectEqualUInt(groupList2NameOffset, 40, "Name offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_GROUP_LIST2::PacketData::Name), 16, "Name size") && ok;
    return ok;
}

auto testGAttrBitfields() -> bool
{
    GP_GROUP_ATTR attr{};
    attr.PartyNo           = 2;
    attr.PartyLeaderFlg    = 1;
    attr.AllianceLeaderFlg = 1;
    attr.PartyRFlg         = 1;
    attr.AllianceRFlg      = 1;
    attr.unknown06         = 1;
    attr.unknown07         = 1;
    attr.unused            = 0x123456;

    std::uint32_t raw = 0;
    std::memcpy(&raw, &attr, sizeof(raw));

    GP_GROUP_ATTR decoded{};
    const auto    expectedRaw = std::uint32_t{ 0x123456FE };
    std::memcpy(&decoded, &expectedRaw, sizeof(expectedRaw));

    bool ok = true;
    ok      = expectEqualUInt(raw, expectedRaw, "encoded GP_GROUP_ATTR bits") && ok;
    ok      = expectEqualUInt(decoded.PartyNo, 2, "GP_GROUP_ATTR PartyNo") && ok;
    ok      = expectEqualUInt(decoded.PartyLeaderFlg, 1, "GP_GROUP_ATTR PartyLeaderFlg") && ok;
    ok      = expectEqualUInt(decoded.AllianceLeaderFlg, 1, "GP_GROUP_ATTR AllianceLeaderFlg") && ok;
    ok      = expectEqualUInt(decoded.PartyRFlg, 1, "GP_GROUP_ATTR PartyRFlg") && ok;
    ok      = expectEqualUInt(decoded.AllianceRFlg, 1, "GP_GROUP_ATTR AllianceRFlg") && ok;
    ok      = expectEqualUInt(decoded.unknown06, 1, "GP_GROUP_ATTR unknown06") && ok;
    ok      = expectEqualUInt(decoded.unknown07, 1, "GP_GROUP_ATTR unknown07") && ok;
    ok      = expectEqualUInt(decoded.unused, 0x123456, "GP_GROUP_ATTR unused") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_GROUP_LIST2();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0xE2, 0x1C, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0E2, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), groupList2PacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded prefix") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), groupList2PacketSize, "payload") && ok;
    ok      = expectZeroRange(packet, groupList2PacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

} // namespace

auto runS2CGroupList2PacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testGAttrBitfields() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
