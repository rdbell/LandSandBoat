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

#include "test_c2s_login_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/entities/char_entity.h"
#include "map/map_session.h"
#include "map/packets/c2s/0x00a_login.h"

namespace
{

constexpr auto loginPacketCheckOffset = 4U;
constexpr auto loginPadding00Offset   = 5U;
constexpr auto loginUnknown00Offset   = 6U;
constexpr auto loginUnknown01Offset   = 8U;
constexpr auto loginUniqueNoOffset    = 12U;
constexpr auto loginGrapIDTblOffset   = 16U;
constexpr auto loginGrapIDCount       = 9U;
constexpr auto loginSNameOffset       = 34U;
constexpr auto loginSNameLen          = 15U;
constexpr auto loginSAccuntOffset     = 49U;
constexpr auto loginSAccuntLen        = 15U;
constexpr auto loginTicketOffset      = 64U;
constexpr auto loginTicketLen         = 16U;
constexpr auto loginVerOffset         = 80U;
constexpr auto loginSPlatformOffset   = 84U;
constexpr auto loginSPlatformLen      = 4U;
constexpr auto loginUCliLangOffset    = 88U;
constexpr auto loginDammyAreaOffset   = 90U;

using LoginBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_LOGIN)>;

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s LOGIN packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s LOGIN packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s LOGIN packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s LOGIN packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s LOGIN packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s LOGIN packet self-test failed: " << label << " got";
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

auto sampleLoginSName() -> std::array<std::uint8_t, loginSNameLen>
{
    return {
        'P',
        'l',
        'a',
        'y',
        'e',
        'r',
        0x00,
        0xFE,
        0xFD,
        'N',
        'a',
        'm',
        'e',
        0x80,
        0x00,
    };
}

auto sampleLoginSAccunt() -> std::array<std::uint8_t, loginSAccuntLen>
{
    return {
        'A',
        'c',
        'c',
        'o',
        'u',
        'n',
        't',
        0x00,
        0xFE,
        0xFD,
        '0',
        '1',
        '2',
        0x80,
        0x00,
    };
}

auto sampleLoginTicket() -> std::array<std::uint8_t, loginTicketLen>
{
    return {
        0x30,
        0x31,
        0x32,
        0x33,
        0x34,
        0x35,
        0x36,
        0x37,
        0x38,
        0x39,
        0x3A,
        0x3B,
        0x3C,
        0x3D,
        0x3E,
        0x3F,
    };
}

auto makeLoginPacket() -> GP_CLI_COMMAND_LOGIN
{
    auto packet             = GP_CLI_COMMAND_LOGIN{};
    packet.header.id       = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_LOGIN);
    packet.header.size     = sizeof(packet) / 4U;
    packet.header.sync     = 0xBEEF;
    packet.LoginPacketCheck = 0x11;
    packet.padding00       = 0x12;
    packet.unknown00       = 0x1314;
    packet.unknown01       = 0x15161718;
    packet.UniqueNo        = 0x191A1B1C;
    for (std::size_t i = 0; i < loginGrapIDCount; ++i)
    {
        packet.GrapIDTbl[i] = static_cast<std::uint16_t>(0x2000U + i);
    }

    const auto sName = sampleLoginSName();
    std::memcpy(packet.sName, sName.data(), sName.size());
    const auto sAccunt = sampleLoginSAccunt();
    std::memcpy(packet.sAccunt, sAccunt.data(), sAccunt.size());
    const auto ticket = sampleLoginTicket();
    std::memcpy(packet.Ticket, ticket.data(), ticket.size());
    packet.Ver = 0x41424344;
    const auto platform = std::array<std::uint8_t, loginSPlatformLen>{ 'W', 'i', 'n', 0x00 };
    std::memcpy(packet.sPlatform, platform.data(), platform.size());
    packet.uCliLang  = 0x4546;
    packet.dammyArea = 0x4748;
    return packet;
}

auto makeExpectedLoginBytes() -> LoginBytes
{
    auto bytes = LoginBytes{};
    bytes[0] = 0x0A;
    bytes[1] = 0x2E;
    bytes[2] = 0xEF;
    bytes[3] = 0xBE;
    bytes[loginPacketCheckOffset] = 0x11;
    bytes[loginPadding00Offset] = 0x12;
    bytes[loginUnknown00Offset] = 0x14;
    bytes[loginUnknown00Offset + 1U] = 0x13;
    bytes[loginUnknown01Offset] = 0x18;
    bytes[loginUnknown01Offset + 1U] = 0x17;
    bytes[loginUnknown01Offset + 2U] = 0x16;
    bytes[loginUnknown01Offset + 3U] = 0x15;
    bytes[loginUniqueNoOffset] = 0x1C;
    bytes[loginUniqueNoOffset + 1U] = 0x1B;
    bytes[loginUniqueNoOffset + 2U] = 0x1A;
    bytes[loginUniqueNoOffset + 3U] = 0x19;
    for (std::size_t i = 0; i < loginGrapIDCount; ++i)
    {
        bytes[loginGrapIDTblOffset + i * 2U] = static_cast<std::uint8_t>(i);
        bytes[loginGrapIDTblOffset + i * 2U + 1U] = 0x20;
    }

    const auto sName = sampleLoginSName();
    std::memcpy(bytes.data() + loginSNameOffset, sName.data(), sName.size());
    const auto sAccunt = sampleLoginSAccunt();
    std::memcpy(bytes.data() + loginSAccuntOffset, sAccunt.data(), sAccunt.size());
    const auto ticket = sampleLoginTicket();
    std::memcpy(bytes.data() + loginTicketOffset, ticket.data(), ticket.size());
    bytes[loginVerOffset] = 0x44;
    bytes[loginVerOffset + 1U] = 0x43;
    bytes[loginVerOffset + 2U] = 0x42;
    bytes[loginVerOffset + 3U] = 0x41;
    const auto platform = std::array<std::uint8_t, loginSPlatformLen>{ 'W', 'i', 'n', 0x00 };
    std::memcpy(bytes.data() + loginSPlatformOffset, platform.data(), platform.size());
    bytes[loginUCliLangOffset] = 0x46;
    bytes[loginUCliLangOffset + 1U] = 0x45;
    bytes[loginDammyAreaOffset] = 0x48;
    bytes[loginDammyAreaOffset + 1U] = 0x47;
    return bytes;
}

auto validateLoginPacket(const GP_CLI_COMMAND_LOGIN& packet, std::uint32_t characterId, bool alreadyLoggedIn) -> PacketValidationResult
{
    auto session    = MapSession{};
    auto character  = CCharEntity{};
    character.id    = characterId;
    character.status = STATUS_TYPE::DISAPPEAR;
    if (alreadyLoggedIn)
    {
        session.blowfish.status      = BLOWFISH_ACCEPTED;
        session.hasDecryptedPacket   = true;
        character.status             = STATUS_TYPE::NORMAL;
    }
    return packet.validate(&session, &character);
}

auto testLoginLayoutAndMetadata() -> bool
{
    bool ok = true;
    const auto packet = GP_CLI_COMMAND_LOGIN{};

    ok = expectEqualString(GP_CLI_COMMAND_LOGIN::name, "GP_CLI_COMMAND_LOGIN", "LOGIN name") && ok;
    ok = expectEqualUInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_LOGIN::packetId), 0x00A, "LOGIN packet id") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_HEADER), 4, "client header size") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_COMMAND_LOGIN), 92, "LOGIN sizeof") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, LoginPacketCheck), loginPacketCheckOffset, "LoginPacketCheck offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, padding00), loginPadding00Offset, "padding00 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, unknown00), loginUnknown00Offset, "unknown00 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, unknown01), loginUnknown01Offset, "unknown01 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, UniqueNo), loginUniqueNoOffset, "UniqueNo offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, GrapIDTbl), loginGrapIDTblOffset, "GrapIDTbl offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, sName), loginSNameOffset, "sName offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, sAccunt), loginSAccuntOffset, "sAccunt offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, Ticket), loginTicketOffset, "Ticket offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, Ver), loginVerOffset, "Ver offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, sPlatform), loginSPlatformOffset, "sPlatform offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, uCliLang), loginUCliLangOffset, "uCliLang offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_LOGIN, dammyArea), loginDammyAreaOffset, "dammyArea offset") && ok;
    ok = expectEqualUInt(sizeof(packet.GrapIDTbl) / sizeof(packet.GrapIDTbl[0]), loginGrapIDCount, "GrapIDTbl count") && ok;
    ok = expectEqualUInt(sizeof(packet.sName), loginSNameLen, "sName len") && ok;
    ok = expectEqualUInt(sizeof(packet.sAccunt), loginSAccuntLen, "sAccunt len") && ok;
    ok = expectEqualUInt(sizeof(packet.Ticket), loginTicketLen, "Ticket len") && ok;
    ok = expectEqualUInt(sizeof(packet.sPlatform), loginSPlatformLen, "sPlatform len") && ok;
    return ok;
}

auto testLoginEncodedBytes() -> bool
{
    return expectBytes(encodedPacketBytes(makeLoginPacket()), makeExpectedLoginBytes(), "LOGIN bytes");
}

auto testLoginValidation() -> bool
{
    bool ok = true;
    auto packet = makeLoginPacket();

    ok = expectValid(validateLoginPacket(packet, packet.UniqueNo, false), "matching character id") && ok;
    ok = expectInvalidError(validateLoginPacket(packet, packet.UniqueNo + 1U, false), "Player ID mismatch", "character id mismatch") && ok;
    ok = expectInvalidError(validateLoginPacket(packet, packet.UniqueNo, true), "Player already logged in.", "already logged in") && ok;

    return ok;
}

} // namespace

auto runC2SLoginPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLoginLayoutAndMetadata() && ok;
    ok      = testLoginEncodedBytes() && ok;
    ok      = testLoginValidation() && ok;
    return ok;
}
