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

#include "test_s2c_group_attr_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0df_group_attr.h"

namespace
{

using GroupAttrPacket = GP_SERV_COMMAND_GROUP_ATTR;

constexpr auto groupAttrUniqueNoOffset          = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, UniqueNo);
constexpr auto groupAttrHpOffset                = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, Hp);
constexpr auto groupAttrMpOffset                = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, Mp);
constexpr auto groupAttrTpOffset                = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, Tp);
constexpr auto groupAttrActIndexOffset          = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, ActIndex);
constexpr auto groupAttrHppOffset               = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, Hpp);
constexpr auto groupAttrMppOffset               = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, Mpp);
constexpr auto groupAttrKindOffset              = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, Kind);
constexpr auto groupAttrMoghouseFlgOffset       = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, MoghouseFlg);
constexpr auto groupAttrZoneNoOffset            = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, ZoneNo);
constexpr auto groupAttrMonstrosityFlagOffset   = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, MonstrosityFlag);
constexpr auto groupAttrMonstrosityNameIdOffset = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, MonstrosityNameId);
constexpr auto groupAttrMJobNoOffset            = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, mjob_no);
constexpr auto groupAttrMJobLvOffset            = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, mjob_lv);
constexpr auto groupAttrSJobNoOffset            = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, sjob_no);
constexpr auto groupAttrSJobLvOffset            = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, sjob_lv);
constexpr auto groupAttrMasterJobLvOffset       = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, masterjob_lv);
constexpr auto groupAttrMasterJobFlagsOffset    = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, masterjob_flags);
constexpr auto groupAttrPadding26Offset         = sizeof(GP_SERV_HEADER) + offsetof(GroupAttrPacket::PacketData, padding26);
constexpr auto groupAttrPadding26Size           = sizeof(GroupAttrPacket::PacketData{}.padding26);
constexpr auto groupAttrPacketDataSize          = sizeof(GroupAttrPacket::PacketData);
constexpr auto groupAttrPacketSize              = sizeof(GP_SERV_HEADER) + groupAttrPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_ATTR packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_ATTR packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_GROUP_ATTR), 0x0DF, "GROUP_ATTR packet id") && ok;
    ok      = expectEqualUInt(groupAttrPadding26Size, 2, "padding26 size") && ok;
    ok      = expectEqualUInt(groupAttrPacketDataSize, 36, "sizeof(GROUP_ATTR::PacketData)") && ok;
    ok      = expectEqualUInt(groupAttrPacketSize, 40, "GROUP_ATTR packet size") && ok;
    ok      = expectEqualUInt(groupAttrUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(groupAttrHpOffset, 8, "Hp offset") && ok;
    ok      = expectEqualUInt(groupAttrMpOffset, 12, "Mp offset") && ok;
    ok      = expectEqualUInt(groupAttrTpOffset, 16, "Tp offset") && ok;
    ok      = expectEqualUInt(groupAttrActIndexOffset, 20, "ActIndex offset") && ok;
    ok      = expectEqualUInt(groupAttrHppOffset, 22, "Hpp offset") && ok;
    ok      = expectEqualUInt(groupAttrMppOffset, 23, "Mpp offset") && ok;
    ok      = expectEqualUInt(groupAttrKindOffset, 24, "Kind offset") && ok;
    ok      = expectEqualUInt(groupAttrMoghouseFlgOffset, 25, "MoghouseFlg offset") && ok;
    ok      = expectEqualUInt(groupAttrZoneNoOffset, 26, "ZoneNo offset") && ok;
    ok      = expectEqualUInt(groupAttrMonstrosityFlagOffset, 28, "MonstrosityFlag offset") && ok;
    ok      = expectEqualUInt(groupAttrMonstrosityNameIdOffset, 30, "MonstrosityNameId offset") && ok;
    ok      = expectEqualUInt(groupAttrMJobNoOffset, 32, "mjob_no offset") && ok;
    ok      = expectEqualUInt(groupAttrMJobLvOffset, 33, "mjob_lv offset") && ok;
    ok      = expectEqualUInt(groupAttrSJobNoOffset, 34, "sjob_no offset") && ok;
    ok      = expectEqualUInt(groupAttrSJobLvOffset, 35, "sjob_lv offset") && ok;
    ok      = expectEqualUInt(groupAttrMasterJobLvOffset, 36, "masterjob_lv offset") && ok;
    ok      = expectEqualUInt(groupAttrMasterJobFlagsOffset, 37, "masterjob_flags offset") && ok;
    ok      = expectEqualUInt(groupAttrPadding26Offset, 38, "padding26 offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data                = GroupAttrPacket::PacketData{};
    data.UniqueNo            = 0x11223344;
    data.Hp                  = 0x55667788;
    data.Mp                  = 0x99AABBCC;
    data.Tp                  = 0xDDEEFF00;
    data.ActIndex            = 0x1357;
    data.Hpp                 = 0x01;
    data.Mpp                 = 0x02;
    data.Kind                = 0x03;
    data.MoghouseFlg         = 0x04;
    data.ZoneNo              = 0x2468;
    data.MonstrosityFlag     = 0x79AC;
    data.MonstrosityNameId   = 0xBEEF;
    data.mjob_no             = 0x05;
    data.mjob_lv             = 0x06;
    data.sjob_no             = 0x07;
    data.sjob_lv             = 0x08;
    data.masterjob_lv        = 0x09;
    data.masterjob_flags     = 0x0A;
    data.padding26[0]        = 0x0B;
    data.padding26[1]        = 0x0C;

    auto expected = std::array<uint8, 36>{};
    expected[0]   = 0x44;
    expected[1]   = 0x33;
    expected[2]   = 0x22;
    expected[3]   = 0x11;
    expected[4]   = 0x88;
    expected[5]   = 0x77;
    expected[6]   = 0x66;
    expected[7]   = 0x55;
    expected[8]   = 0xCC;
    expected[9]   = 0xBB;
    expected[10]  = 0xAA;
    expected[11]  = 0x99;
    expected[12]  = 0x00;
    expected[13]  = 0xFF;
    expected[14]  = 0xEE;
    expected[15]  = 0xDD;
    expected[16]  = 0x57;
    expected[17]  = 0x13;
    expected[18]  = 0x01;
    expected[19]  = 0x02;
    expected[20]  = 0x03;
    expected[21]  = 0x04;
    expected[22]  = 0x68;
    expected[23]  = 0x24;
    expected[24]  = 0xAC;
    expected[25]  = 0x79;
    expected[26]  = 0xEF;
    expected[27]  = 0xBE;
    expected[28]  = 0x05;
    expected[29]  = 0x06;
    expected[30]  = 0x07;
    expected[31]  = 0x08;
    expected[32]  = 0x09;
    expected[33]  = 0x0A;
    expected[34]  = 0x0B;
    expected[35]  = 0x0C;

    return expectStructBytes(data, expected, "GROUP_ATTR PacketData bytes");
}

} // namespace

auto runS2CGroupAttrPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
