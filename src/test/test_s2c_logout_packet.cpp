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

#include "test_s2c_logout_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/ipp.h"
#include "map/packets/s2c/0x00b_logout.h"

namespace
{

constexpr auto logoutStateOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_LOGOUT::PacketData, LogoutState);
constexpr auto logoutIwasakiOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_LOGOUT::PacketData, Iwasaki);
constexpr auto logoutIwasakiIPOffset      = logoutIwasakiOffset + offsetof(GP_SERV_LOGOUTSUB, ip);
constexpr auto logoutIwasakiPortOffset    = logoutIwasakiOffset + offsetof(GP_SERV_LOGOUTSUB, port);
constexpr auto logoutIwasakiPaddingOffset = logoutIwasakiOffset + offsetof(GP_SERV_LOGOUTSUB, padding00);
constexpr auto logoutCliErrCodeOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_LOGOUT::PacketData, cliErrCode);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c LOGOUT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(CBasicPacket& packet, const std::array<uint8, 28>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c LOGOUT packet self-test failed: " << label << " got";
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

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c LOGOUT packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_LOGOUTSUB), 16, "sizeof(GP_SERV_LOGOUTSUB)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_LOGOUT::PacketData), 24, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(logoutStateOffset, 4, "LogoutState offset") && ok;
    ok      = expectEqualUInt(logoutIwasakiOffset, 8, "Iwasaki offset") && ok;
    ok      = expectEqualUInt(logoutIwasakiIPOffset, 8, "Iwasaki.ip offset") && ok;
    ok      = expectEqualUInt(logoutIwasakiPortOffset, 12, "Iwasaki.port offset") && ok;
    ok      = expectEqualUInt(logoutIwasakiPaddingOffset, 16, "Iwasaki.padding00 offset") && ok;
    ok      = expectEqualUInt(logoutCliErrCodeOffset, 24, "cliErrCode offset") && ok;
    return ok;
}

auto testEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::NONE), 0, "NONE") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::LOGOUT), 1, "LOGOUT") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::ZONECHANGE), 2, "ZONECHANGE") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::MYROOM), 3, "MYROOM") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::CANCELL), 4, "CANCELL") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::POLEXIT), 5, "POLEXIT") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::JOBEXIT), 6, "JOBEXIT") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::POLEXIT_MYROOM), 7, "POLEXIT_MYROOM") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::TIMEOUT), 8, "TIMEOUT") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::GMLOGOUT), 9, "GMLOGOUT") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_GAME_LOGOUT_STATE::END), 10, "END") && ok;

    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::NOERR), 0, "NOERR") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::RESERR), 1, "RESERR") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::ZONEDOWN), 2, "ZONEDOWN") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::REGERR), 3, "REGERR") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::CLIVERSERR), 4, "CLIVERSERR") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::CLINOEXERR), 5, "CLINOEXERR") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::UNKNOWN), 6, "UNKNOWN") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_GAME_ECODE::MAX), 7, "MAX") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    const auto zoneIPP = IPP(755083456U, 54230U);
    auto       packet  = GP_SERV_COMMAND_LOGOUT(GP_GAME_LOGOUT_STATE::ZONECHANGE, zoneIPP);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 28>{
        0x0B, 0x0E, 0xEF, 0xBE,
        0x02, 0x00, 0x00, 0x00,
        0xC0, 0xA8, 0x01, 0x2D,
        0xD6, 0xD3, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    const auto decodedIPP = packet.zoneIPP();

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x00B, "LOGOUT type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "LOGOUT size") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(packet.zoneType()), 2, "zoneType") && ok;
    ok      = expectEqualUInt(decodedIPP.getIP(), zoneIPP.getIP(), "zoneIPP ip") && ok;
    ok      = expectEqualUInt(decodedIPP.getPort(), zoneIPP.getPort(), "zoneIPP port") && ok;
    ok      = expectBytes(packet, expected, "encoded LOGOUT prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "LOGOUT tail") && ok;
    return ok;
}

} // namespace

auto runS2CLogoutPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEnums() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
