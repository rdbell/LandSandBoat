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

#include "test_s2c_clistatus_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>

#include "map/packets/s2c/0x061_clistatus.h"
#include "map/packets/s2c/clistatus_runtime.h"

namespace
{

using Packet = GP_SERV_COMMAND_CLISTATUS;

constexpr auto cliStatusStatusDataOffset         = sizeof(GP_SERV_HEADER) + offsetof(Packet::PacketData, statusdata);
constexpr auto cliStatusHPMaxOffset              = cliStatusStatusDataOffset + offsetof(CLISTATUS, hpmax);
constexpr auto cliStatusMPMaxOffset              = cliStatusStatusDataOffset + offsetof(CLISTATUS, mpmax);
constexpr auto cliStatusMJobNoOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, mjob_no);
constexpr auto cliStatusMJobLvOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, mjob_lv);
constexpr auto cliStatusSJobNoOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, sjob_no);
constexpr auto cliStatusSJobLvOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, sjob_lv);
constexpr auto cliStatusExpNowOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, exp_now);
constexpr auto cliStatusExpNextOffset            = cliStatusStatusDataOffset + offsetof(CLISTATUS, exp_next);
constexpr auto cliStatusBPBaseOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, bp_base);
constexpr auto cliStatusBPAdjOffset              = cliStatusStatusDataOffset + offsetof(CLISTATUS, bp_adj);
constexpr auto cliStatusAtkOffset                = cliStatusStatusDataOffset + offsetof(CLISTATUS, atk);
constexpr auto cliStatusDefOffset                = cliStatusStatusDataOffset + offsetof(CLISTATUS, def);
constexpr auto cliStatusDefElemOffset            = cliStatusStatusDataOffset + offsetof(CLISTATUS, def_elem);
constexpr auto cliStatusDesignationOffset        = cliStatusStatusDataOffset + offsetof(CLISTATUS, designation);
constexpr auto cliStatusRankOffset               = cliStatusStatusDataOffset + offsetof(CLISTATUS, rank);
constexpr auto cliStatusRankBarOffset            = cliStatusStatusDataOffset + offsetof(CLISTATUS, rankbar);
constexpr auto cliStatusBindZoneNoOffset         = cliStatusStatusDataOffset + offsetof(CLISTATUS, BindZoneNo);
constexpr auto cliStatusMonsterBusterOffset      = cliStatusStatusDataOffset + offsetof(CLISTATUS, MonsterBuster);
constexpr auto cliStatusNationOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, nation);
constexpr auto cliStatusMyRoomOffset             = cliStatusStatusDataOffset + offsetof(CLISTATUS, myroom);
constexpr auto cliStatusSuperiorLevelOffset      = cliStatusStatusDataOffset + offsetof(CLISTATUS, su_lv);
constexpr auto cliStatusPadding4FOffset          = cliStatusStatusDataOffset + offsetof(CLISTATUS, padding4F);
constexpr auto cliStatusHighestItemLevelOffset   = cliStatusStatusDataOffset + offsetof(CLISTATUS, highest_ilvl);
constexpr auto cliStatusItemLevelOffset          = cliStatusStatusDataOffset + offsetof(CLISTATUS, ilvl);
constexpr auto cliStatusMainhandItemLevelOffset  = cliStatusStatusDataOffset + offsetof(CLISTATUS, ilvl_mhand);
constexpr auto cliStatusRangedItemLevelOffset    = cliStatusStatusDataOffset + offsetof(CLISTATUS, ilvl_ranged);
constexpr auto cliStatusUnityInfoOffset          = cliStatusStatusDataOffset + offsetof(CLISTATUS, unity_info);
constexpr auto cliStatusUnityPoints1Offset       = cliStatusStatusDataOffset + offsetof(CLISTATUS, unity_points1);
constexpr auto cliStatusUnityPoints2Offset       = cliStatusStatusDataOffset + offsetof(CLISTATUS, unity_points2);
constexpr auto cliStatusUnityChatColorFlagOffset = cliStatusStatusDataOffset + offsetof(CLISTATUS, unity_chat_color_flag);
constexpr auto cliStatusMasteryInfoOffset        = cliStatusStatusDataOffset + offsetof(CLISTATUS, mastery_info);
constexpr auto cliStatusMasteryExpNowOffset      = cliStatusStatusDataOffset + offsetof(CLISTATUS, mastery_exp_now);
constexpr auto cliStatusMasteryExpNextOffset     = cliStatusStatusDataOffset + offsetof(CLISTATUS, mastery_exp_next);
constexpr auto cliStatusPacketDataSize           = sizeof(Packet::PacketData);
constexpr auto cliStatusPacketSize               = sizeof(GP_SERV_HEADER) + cliStatusPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CLISTATUS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(const uint8* data, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c CLISTATUS packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_CLISTATUS), 0x061, "CLISTATUS packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(unityinfo_t), 4, "sizeof(unityinfo_t)") && ok;
    ok      = expectEqualUInt(sizeof(masteryinfo_t), 4, "sizeof(masteryinfo_t)") && ok;
    ok      = expectEqualUInt(sizeof(CLISTATUS), 108, "sizeof(CLISTATUS)") && ok;
    ok      = expectEqualUInt(cliStatusPacketDataSize, 108, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(cliStatusPacketSize, 112, "packet size") && ok;
    ok      = expectEqualUInt(cliStatusHPMaxOffset, 4, "hpmax offset") && ok;
    ok      = expectEqualUInt(cliStatusMPMaxOffset, 8, "mpmax offset") && ok;
    ok      = expectEqualUInt(cliStatusMJobNoOffset, 12, "mjob_no offset") && ok;
    ok      = expectEqualUInt(cliStatusMJobLvOffset, 13, "mjob_lv offset") && ok;
    ok      = expectEqualUInt(cliStatusSJobNoOffset, 14, "sjob_no offset") && ok;
    ok      = expectEqualUInt(cliStatusSJobLvOffset, 15, "sjob_lv offset") && ok;
    ok      = expectEqualUInt(cliStatusExpNowOffset, 16, "exp_now offset") && ok;
    ok      = expectEqualUInt(cliStatusExpNextOffset, 18, "exp_next offset") && ok;
    ok      = expectEqualUInt(cliStatusBPBaseOffset, 20, "bp_base offset") && ok;
    ok      = expectEqualUInt(cliStatusBPAdjOffset, 34, "bp_adj offset") && ok;
    ok      = expectEqualUInt(cliStatusAtkOffset, 48, "atk offset") && ok;
    ok      = expectEqualUInt(cliStatusDefOffset, 50, "def offset") && ok;
    ok      = expectEqualUInt(cliStatusDefElemOffset, 52, "def_elem offset") && ok;
    ok      = expectEqualUInt(cliStatusDesignationOffset, 68, "designation offset") && ok;
    ok      = expectEqualUInt(cliStatusRankOffset, 70, "rank offset") && ok;
    ok      = expectEqualUInt(cliStatusRankBarOffset, 72, "rankbar offset") && ok;
    ok      = expectEqualUInt(cliStatusBindZoneNoOffset, 74, "BindZoneNo offset") && ok;
    ok      = expectEqualUInt(cliStatusMonsterBusterOffset, 76, "MonsterBuster offset") && ok;
    ok      = expectEqualUInt(cliStatusNationOffset, 80, "nation offset") && ok;
    ok      = expectEqualUInt(cliStatusMyRoomOffset, 81, "myroom offset") && ok;
    ok      = expectEqualUInt(cliStatusSuperiorLevelOffset, 82, "su_lv offset") && ok;
    ok      = expectEqualUInt(cliStatusPadding4FOffset, 83, "padding4F offset") && ok;
    ok      = expectEqualUInt(cliStatusHighestItemLevelOffset, 84, "highest_ilvl offset") && ok;
    ok      = expectEqualUInt(cliStatusItemLevelOffset, 85, "ilvl offset") && ok;
    ok      = expectEqualUInt(cliStatusMainhandItemLevelOffset, 86, "ilvl_mhand offset") && ok;
    ok      = expectEqualUInt(cliStatusRangedItemLevelOffset, 87, "ilvl_ranged offset") && ok;
    ok      = expectEqualUInt(cliStatusUnityInfoOffset, 88, "unity_info offset") && ok;
    ok      = expectEqualUInt(cliStatusUnityPoints1Offset, 92, "unity_points1 offset") && ok;
    ok      = expectEqualUInt(cliStatusUnityPoints2Offset, 94, "unity_points2 offset") && ok;
    ok      = expectEqualUInt(cliStatusUnityChatColorFlagOffset, 96, "unity_chat_color_flag offset") && ok;
    ok      = expectEqualUInt(cliStatusMasteryInfoOffset, 100, "mastery_info offset") && ok;
    ok      = expectEqualUInt(cliStatusMasteryExpNowOffset, 104, "mastery_exp_now offset") && ok;
    ok      = expectEqualUInt(cliStatusMasteryExpNextOffset, 108, "mastery_exp_next offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto  data = Packet::PacketData{};
    auto& s    = data.statusdata;

    s.hpmax    = 0x11223344;
    s.mpmax    = 0x55667788;
    s.mjob_no  = static_cast<JOBTYPE>(0x09);
    s.mjob_lv  = 0x63;
    s.sjob_no  = static_cast<JOBTYPE>(0x0F);
    s.sjob_lv  = 0x31;
    s.exp_now  = -1234;
    s.exp_next = 2345;

    for (std::size_t i = 0; i < std::size(s.bp_base); ++i)
    {
        s.bp_base[i] = static_cast<uint16>(101 + i);
        s.bp_adj[i]  = static_cast<int16>(-7 + static_cast<int>(i));
    }

    s.atk         = -321;
    s.def         = 4321;
    s.def_elem[0] = -80;
    s.def_elem[1] = -70;
    s.def_elem[2] = -60;
    s.def_elem[3] = -50;
    s.def_elem[4] = 50;
    s.def_elem[5] = 60;
    s.def_elem[6] = 70;
    s.def_elem[7] = 80;

    s.designation   = 0x1234;
    s.rank          = 9;
    s.rankbar       = 0x0A0B;
    s.BindZoneNo    = 0x0C0D;
    s.MonsterBuster = 0x01020304;
    s.nation        = 2;
    s.myroom        = 1;
    s.su_lv         = 7;
    s.padding4F     = 0xAA;
    s.highest_ilvl  = 0x78;
    s.ilvl          = 0x76;
    s.ilvl_mhand    = 0x75;
    s.ilvl_ranged   = 0x74;

    s.unity_info.Faction    = 0x12;
    s.unity_info.Unknown    = 0x15;
    s.unity_info.Points     = 0x12345;
    s.unity_points1         = 0x2222;
    s.unity_points2         = 0x3333;
    s.unity_chat_color_flag = 1;

    s.mastery_info.job_no    = 0x0A;
    s.mastery_info.job_lv    = 0x63;
    s.mastery_info.flags     = 0x05;
    s.mastery_info.padding00 = 0xEE;
    s.mastery_exp_now        = 0x11223344;
    s.mastery_exp_next       = 0x55667788;

    const auto expected = std::array<uint8, cliStatusPacketDataSize>{
        0x44,
        0x33,
        0x22,
        0x11,
        0x88,
        0x77,
        0x66,
        0x55,
        0x09,
        0x63,
        0x0F,
        0x31,
        0x2E,
        0xFB,
        0x29,
        0x09,
        0x65,
        0x00,
        0x66,
        0x00,
        0x67,
        0x00,
        0x68,
        0x00,
        0x69,
        0x00,
        0x6A,
        0x00,
        0x6B,
        0x00,
        0xF9,
        0xFF,
        0xFA,
        0xFF,
        0xFB,
        0xFF,
        0xFC,
        0xFF,
        0xFD,
        0xFF,
        0xFE,
        0xFF,
        0xFF,
        0xFF,
        0xBF,
        0xFE,
        0xE1,
        0x10,
        0xB0,
        0xFF,
        0xBA,
        0xFF,
        0xC4,
        0xFF,
        0xCE,
        0xFF,
        0x32,
        0x00,
        0x3C,
        0x00,
        0x46,
        0x00,
        0x50,
        0x00,
        0x34,
        0x12,
        0x09,
        0x00,
        0x0B,
        0x0A,
        0x0D,
        0x0C,
        0x04,
        0x03,
        0x02,
        0x01,
        0x02,
        0x01,
        0x07,
        0xAA,
        0x78,
        0x76,
        0x75,
        0x74,
        0xB2,
        0x16,
        0x8D,
        0x04,
        0x22,
        0x22,
        0x33,
        0x33,
        0x01,
        0x00,
        0x00,
        0x00,
        0x0A,
        0x63,
        0x05,
        0xEE,
        0x44,
        0x33,
        0x22,
        0x11,
        0x88,
        0x77,
        0x66,
        0x55,
    };

    return expectBytes(reinterpret_cast<const uint8*>(&data), expected, "PacketData bytes");
}

auto testRuntimeShaping() -> bool
{
    auto facts            = clistatushelpers::Facts{};
    facts.status.hpmax    = 1234;
    facts.status.mpmax    = 567;
    facts.status.mjob_no  = static_cast<JOBTYPE>(3);
    facts.status.mjob_lv  = 99;
    facts.status.sjob_no  = static_cast<JOBTYPE>(4);
    facts.status.sjob_lv  = 49;
    facts.status.exp_now  = 456;
    facts.status.exp_next = 789;
    const auto baseParams = std::array<uint16, 7>{ 500, 7, 8, 9, 10, 11, 12 };
    std::copy(baseParams.begin(), baseParams.end(), std::begin(facts.status.bp_base));
    facts.status.MonsterBuster    = 0xFFFFFFFF;
    facts.status.myroom           = 0xFF;
    facts.status.padding4F        = 0xFF;
    facts.status.mastery_exp_now  = 0xFFFFFFFF;
    facts.status.mastery_exp_next = 0xFFFFFFFF;
    facts.baseParamModifiers      = { 800, -2000, 3, 4, 5, 6, 7 };
    facts.twoHandedSTR            = 500;
    facts.mainWeaponTwoHanded     = true;
    facts.unityLeader             = 2;
    facts.unityRank               = 7;
    facts.unityAccolades          = 0x12345;
    facts.currentAccolades        = 123456;
    facts.previousAccolades       = 234567;
    facts.unityChat               = true;

    const auto  packet = clistatushelpers::PlanFor(facts);
    const auto& status = packet.statusdata;

    bool ok = true;
    ok      = expectEqualUInt(status.hpmax, 1234, "runtime hpmax") && ok;
    ok      = expectEqualUInt(status.mjob_no, 3, "runtime main job") && ok;
    ok      = expectEqualUInt(status.bp_adj[0], 499, "runtime two-handed STR clamp") && ok;
    ok      = expectEqualUInt(status.bp_adj[1], static_cast<uint64>(-992), "runtime base adjustment clamp") && ok;
    ok      = expectEqualUInt(status.bp_adj[6], 7, "runtime base adjustment") && ok;
    ok      = expectEqualUInt(status.MonsterBuster, 0, "runtime MonsterBuster remains zero") && ok;
    ok      = expectEqualUInt(status.myroom, 0, "runtime myroom remains zero") && ok;
    ok      = expectEqualUInt(status.padding4F, 0, "runtime padding remains zero") && ok;
    ok      = expectEqualUInt(status.unity_info.Faction, 2, "runtime unity faction") && ok;
    ok      = expectEqualUInt(status.unity_info.Unknown, 7, "runtime unity rank") && ok;
    ok      = expectEqualUInt(status.unity_info.Points, 0x12345, "runtime unity points") && ok;
    ok      = expectEqualUInt(status.unity_points1, 123, "runtime current accolades") && ok;
    ok      = expectEqualUInt(status.unity_points2, 234, "runtime previous accolades") && ok;
    ok      = expectEqualUInt(status.unity_chat_color_flag, 1, "runtime unity chat") && ok;
    ok      = expectEqualUInt(status.mastery_exp_now, 0, "runtime mastery experience remains zero") && ok;
    ok      = expectEqualUInt(status.mastery_exp_next, 0, "runtime mastery next remains zero") && ok;
    return ok;
}

} // namespace

auto runS2CCliStatusPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testRuntimeShaping() && ok;
    return ok;
}
