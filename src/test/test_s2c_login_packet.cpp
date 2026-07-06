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

#include "test_s2c_login_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x00a_login.h"

namespace
{

using LoginPacket = GP_SERV_COMMAND_LOGIN;

constexpr auto loginPosHeadSize        = sizeof(GP_SERV_POS_HEAD);
constexpr auto loginDancerSize         = sizeof(GP_MYROOM_DANCER_PKT);
constexpr auto loginSaveConfSize       = sizeof(SAVE_CONF_PKT);
constexpr auto loginPacketDataSize     = sizeof(LoginPacket::PacketData);
constexpr auto loginPacketSize         = sizeof(GP_SERV_HEADER) + loginPacketDataSize;
constexpr auto loginPosHeadOffset      = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead);
constexpr auto loginUniqueNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, UniqueNo);
constexpr auto loginActIndexOffset     = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, ActIndex);
constexpr auto loginDirOffset          = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, dir);
constexpr auto loginXOffset            = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, x);
constexpr auto loginZOffset            = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, z);
constexpr auto loginYOffset            = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, y);
constexpr auto loginFlags1Offset       = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, flags1);
constexpr auto loginSpeedOffset        = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, Speed);
constexpr auto loginFlags2Offset       = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, PosHead) + offsetof(GP_SERV_POS_HEAD, flags2);
constexpr auto loginZoneNoOffset       = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, ZoneNo);
constexpr auto loginEventNoOffset      = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, EventNo);
constexpr auto loginGrapIDTblOffset    = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, GrapIDTbl);
constexpr auto loginMusicNumOffset     = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, MusicNum);
constexpr auto loginWeatherNumberOffset = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, WeatherNumber);
constexpr auto loginLoginStateOffset    = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, LoginState);
constexpr auto loginNameOffset          = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, name);
constexpr auto loginCertificateOffset   = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, certificate);
constexpr auto loginZoneSubNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, ZoneSubNo);
constexpr auto loginMyroomSubMapOffset  = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, MyroomSubMapNumber);
constexpr auto loginMyroomMapOffset     = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, MyroomMapNumber);
constexpr auto loginDancerOffset        = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, Dancer);
constexpr auto loginConfDataOffset      = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, ConfData);
constexpr auto loginExOffset            = sizeof(GP_SERV_HEADER) + offsetof(LoginPacket::PacketData, Ex);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c LOGIN packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c LOGIN packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_LOGIN), 0x00A, "LOGIN packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(loginPosHeadSize, 44, "sizeof(GP_SERV_POS_HEAD)") && ok;
    ok      = expectEqualUInt(loginDancerSize, 68, "sizeof(GP_MYROOM_DANCER_PKT)") && ok;
    ok      = expectEqualUInt(loginSaveConfSize, 12, "sizeof(SAVE_CONF_PKT)") && ok;
    ok      = expectEqualUInt(loginPacketDataSize, 256, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(loginPacketSize, 260, "packet size") && ok;
    ok      = expectEqualUInt(loginPosHeadOffset, 4, "PosHead offset") && ok;
    ok      = expectEqualUInt(loginUniqueNoOffset, 4, "PosHead.UniqueNo offset") && ok;
    ok      = expectEqualUInt(loginActIndexOffset, 8, "PosHead.ActIndex offset") && ok;
    ok      = expectEqualUInt(loginDirOffset, 11, "PosHead.dir offset") && ok;
    ok      = expectEqualUInt(loginXOffset, 12, "PosHead.x offset") && ok;
    ok      = expectEqualUInt(loginZOffset, 16, "PosHead.z offset") && ok;
    ok      = expectEqualUInt(loginYOffset, 20, "PosHead.y offset") && ok;
    ok      = expectEqualUInt(loginFlags1Offset, 24, "PosHead.flags1 offset") && ok;
    ok      = expectEqualUInt(loginSpeedOffset, 28, "PosHead.Speed offset") && ok;
    ok      = expectEqualUInt(loginFlags2Offset, 32, "PosHead.flags2 offset") && ok;
    ok      = expectEqualUInt(loginZoneNoOffset, 48, "ZoneNo offset") && ok;
    ok      = expectEqualUInt(loginEventNoOffset, 64, "EventNo offset") && ok;
    ok      = expectEqualUInt(loginGrapIDTblOffset, 68, "GrapIDTbl offset") && ok;
    ok      = expectEqualUInt(loginMusicNumOffset, 86, "MusicNum offset") && ok;
    ok      = expectEqualUInt(loginWeatherNumberOffset, 104, "WeatherNumber offset") && ok;
    ok      = expectEqualUInt(loginLoginStateOffset, 128, "LoginState offset") && ok;
    ok      = expectEqualUInt(loginNameOffset, 132, "name offset") && ok;
    ok      = expectEqualUInt(loginCertificateOffset, 148, "certificate offset") && ok;
    ok      = expectEqualUInt(loginZoneSubNoOffset, 158, "ZoneSubNo offset") && ok;
    ok      = expectEqualUInt(loginMyroomSubMapOffset, 168, "MyroomSubMapNumber offset") && ok;
    ok      = expectEqualUInt(loginMyroomMapOffset, 170, "MyroomMapNumber offset") && ok;
    ok      = expectEqualUInt(loginDancerOffset, 176, "Dancer offset") && ok;
    ok      = expectEqualUInt(loginConfDataOffset, 244, "ConfData offset") && ok;
    ok      = expectEqualUInt(loginExOffset, 256, "Ex offset") && ok;
    ok      = expectEqualUInt(sizeof(LoginPacket::PacketData{}.GrapIDTbl), 18, "GrapIDTbl size") && ok;
    ok      = expectEqualUInt(sizeof(LoginPacket::PacketData{}.MusicNum), 10, "MusicNum size") && ok;
    ok      = expectEqualUInt(sizeof(LoginPacket::PacketData{}.name), 16, "name size") && ok;
    ok      = expectEqualUInt(sizeof(LoginPacket::PacketData{}.certificate), 8, "certificate size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER_PKT{}.job_lev), 16, "dancer job_lev size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER_PKT{}.bp_base), 14, "dancer bp_base size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER_PKT{}.bp_adj), 14, "dancer bp_adj size") && ok;
    ok      = expectEqualUInt(sizeof(GP_MYROOM_DANCER_PKT{}.unknown41), 3, "dancer unknown41 size") && ok;
    ok      = expectEqualUInt(sizeof(SAVE_CONF_PKT{}.unknown00), 12, "SAVE_CONF unknown00 size") && ok;
    return ok;
}

auto testLoginStateEnum() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_NONE), 0, "SAVE_LOGIN_STATE_NONE") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_MYROOM), 1, "SAVE_LOGIN_STATE_MYROOM") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_GAME), 2, "SAVE_LOGIN_STATE_GAME") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_POLEXIT), 3, "SAVE_LOGIN_STATE_POLEXIT") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_JOBEXIT), 4, "SAVE_LOGIN_STATE_JOBEXIT") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_POLEXIT_MYROOM), 5, "SAVE_LOGIN_STATE_POLEXIT_MYROOM") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_END), 6, "SAVE_LOGIN_STATE_END") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = LoginPacket::PacketData{};

    data.PosHead.UniqueNo      = 0x01020304;
    data.PosHead.ActIndex      = 0x0506;
    data.PosHead.padding06     = 0x07;
    data.PosHead.dir           = -8;
    data.PosHead.x             = 1.5F;
    data.PosHead.z             = -2.25F;
    data.PosHead.y             = 3.75F;
    data.PosHead.flags1        = 0x11121314;
    data.PosHead.Speed         = 0x15;
    data.PosHead.SpeedBase     = 0x16;
    data.PosHead.HpMax         = 0x17;
    data.PosHead.server_status = 0x18;
    data.PosHead.flags2        = 0x21222324;
    data.PosHead.flags3        = 0x25262728;
    data.PosHead.flags4        = 0x292A2B2C;
    data.PosHead.BtTargetID    = 0x2D2E2F30;
    data.ZoneNo                = 0x31323334;
    data.ntTime                = 0x35363738;
    data.ntTimeSec             = 0x393A3B3C;
    data.GameTime              = 0x3D3E3F40;
    data.EventNo               = 0x4142;
    data.MapNumber             = 0x4344;
    for (std::size_t i = 0; i < 9; ++i)
    {
        data.GrapIDTbl[i] = static_cast<std::uint16_t>(0x5000 + i);
    }
    for (std::size_t i = 0; i < 5; ++i)
    {
        data.MusicNum[i] = static_cast<std::uint16_t>(0x6000 + i);
    }
    data.SubMapNumber     = 0x4546;
    data.EventNum         = 0x4748;
    data.EventPara        = 0x494A;
    data.EventMode        = 0x4B4C;
    data.WeatherNumber    = 0x4D4E;
    data.WeatherNumber2   = 0x4F50;
    data.WeatherTime      = 0x51525354;
    data.WeatherTime2     = 0x55565758;
    data.WeatherOffsetTime = 0x595A5B5C;
    data.ShipStart         = 0x5D5E5F60;
    data.ShipEnd           = 0x6162;
    data.IsMonstrosity     = 0x6364;
    data.LoginState        = SAVE_LOGIN_STATE::SAVE_LOGIN_STATE_POLEXIT_MYROOM;
    std::memcpy(data.name, "PlayerName012345", sizeof(data.name));
    data.certificate[0] = -123456789;
    data.certificate[1] = 0x11223344;
    data.unknown9C      = 0x6566;
    data.ZoneSubNo      = 0x6768;
    data.PlayTime       = 0x696A6B6C;
    data.DeadCounter    = 0x6D6E6F70;
    data.MyroomSubMapNumber = 0x71;
    data.unknownA9          = 0x72;
    data.MyroomMapNumber    = 0x7374;
    data.SendCount          = 0x7576;
    data.MyRoomExitBit      = 0x77;
    data.MogZoneFlag        = 0x78;

    data.Dancer.mon_no       = 0x0102;
    data.Dancer.face_no      = 0x0304;
    data.Dancer.mjob_no      = 0x05;
    data.Dancer.hair_no      = 0x06;
    data.Dancer.size         = 0x07;
    data.Dancer.sjob_no      = 0x08;
    data.Dancer.get_job_flag = 0x11121314;
    for (std::size_t i = 0; i < 16; ++i)
    {
        data.Dancer.job_lev[i] = static_cast<int8_t>(-1 - static_cast<int>(i));
    }
    for (std::size_t i = 0; i < 7; ++i)
    {
        data.Dancer.bp_base[i] = static_cast<std::uint16_t>(0x3132 + (i * 0x0202));
        data.Dancer.bp_adj[i]  = static_cast<int16_t>(-300 - static_cast<int>(i));
    }
    data.Dancer.hpmax      = -12345;
    data.Dancer.mpmax      = 0x51525354;
    data.Dancer.sjobflg    = 0x55;
    data.Dancer.unknown41[0] = 0x56;
    data.Dancer.unknown41[1] = 0x57;
    data.Dancer.unknown41[2] = 0x58;

    data.ConfData.unknown00[0] = 0x81828384;
    data.ConfData.unknown00[1] = 0x85868788;
    data.ConfData.unknown00[2] = 0x898A8B8C;
    data.Ex                    = 0x91929394;

    auto expected = std::array<uint8, loginPacketDataSize>{};
    putLE32(expected, 0, 0x01020304);
    putLE16(expected, 4, 0x0506);
    expected[6] = 0x07;
    expected[7] = 0xF8;
    putLE32(expected, 8, 0x3FC00000);
    putLE32(expected, 12, 0xC0100000);
    putLE32(expected, 16, 0x40700000);
    putLE32(expected, 20, 0x11121314);
    expected[24] = 0x15;
    expected[25] = 0x16;
    expected[26] = 0x17;
    expected[27] = 0x18;
    putLE32(expected, 28, 0x21222324);
    putLE32(expected, 32, 0x25262728);
    putLE32(expected, 36, 0x292A2B2C);
    putLE32(expected, 40, 0x2D2E2F30);
    putLE32(expected, 44, 0x31323334);
    putLE32(expected, 48, 0x35363738);
    putLE32(expected, 52, 0x393A3B3C);
    putLE32(expected, 56, 0x3D3E3F40);
    putLE16(expected, 60, 0x4142);
    putLE16(expected, 62, 0x4344);
    for (std::size_t i = 0; i < 9; ++i)
    {
        putLE16(expected, 64 + (i * 2), static_cast<std::uint16_t>(0x5000 + i));
    }
    for (std::size_t i = 0; i < 5; ++i)
    {
        putLE16(expected, 82 + (i * 2), static_cast<std::uint16_t>(0x6000 + i));
    }
    putLE16(expected, 92, 0x4546);
    putLE16(expected, 94, 0x4748);
    putLE16(expected, 96, 0x494A);
    putLE16(expected, 98, 0x4B4C);
    putLE16(expected, 100, 0x4D4E);
    putLE16(expected, 102, 0x4F50);
    putLE32(expected, 104, 0x51525354);
    putLE32(expected, 108, 0x55565758);
    putLE32(expected, 112, 0x595A5B5C);
    putLE32(expected, 116, 0x5D5E5F60);
    putLE16(expected, 120, 0x6162);
    putLE16(expected, 122, 0x6364);
    putLE32(expected, 124, 5);
    std::memcpy(expected.data() + 128, "PlayerName012345", 16);
    putLE32(expected, 144, static_cast<std::uint32_t>(-123456789));
    putLE32(expected, 148, 0x11223344);
    putLE16(expected, 152, 0x6566);
    putLE16(expected, 154, 0x6768);
    putLE32(expected, 156, 0x696A6B6C);
    putLE32(expected, 160, 0x6D6E6F70);
    expected[164] = 0x71;
    expected[165] = 0x72;
    putLE16(expected, 166, 0x7374);
    putLE16(expected, 168, 0x7576);
    expected[170] = 0x77;
    expected[171] = 0x78;
    putLE16(expected, 172, 0x0102);
    putLE16(expected, 174, 0x0304);
    expected[176] = 0x05;
    expected[177] = 0x06;
    expected[178] = 0x07;
    expected[179] = 0x08;
    putLE32(expected, 180, 0x11121314);
    for (std::size_t i = 0; i < 16; ++i)
    {
        expected[184 + i] = static_cast<uint8>(0xFF - i);
    }
    for (std::size_t i = 0; i < 7; ++i)
    {
        putLE16(expected, 200 + (i * 2), static_cast<std::uint16_t>(0x3132 + (i * 0x0202)));
        putLE16(expected, 214 + (i * 2), static_cast<std::uint16_t>(-300 - static_cast<int>(i)));
    }
    putLE32(expected, 228, static_cast<std::uint32_t>(-12345));
    putLE32(expected, 232, 0x51525354);
    expected[236] = 0x55;
    expected[237] = 0x56;
    expected[238] = 0x57;
    expected[239] = 0x58;
    putLE32(expected, 240, 0x81828384);
    putLE32(expected, 244, 0x85868788);
    putLE32(expected, 248, 0x898A8B8C);
    putLE32(expected, 252, 0x91929394);

    return expectStructBytes(data, expected, "LOGIN PacketData bytes");
}

} // namespace

auto runS2CLoginPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testLoginStateEnum() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
