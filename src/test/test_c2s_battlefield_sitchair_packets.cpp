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

#include "test_c2s_battlefield_sitchair_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x112_battlefield_req.h"
#include "map/packets/c2s/0x113_sitchair.h"

namespace
{

using BattlefieldReqBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BATTLEFIELD_REQ)>;
using SitChairBytes       = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SITCHAIR)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s battlefield/sitchair packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s battlefield/sitchair packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s battlefield/sitchair packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s battlefield/sitchair packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s battlefield/sitchair packet self-test failed: " << label << " got";
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

auto makeBattlefieldReqPacket(std::uint8_t kind, std::array<std::uint8_t, 3> padding) -> GP_CLI_COMMAND_BATTLEFIELD_REQ
{
    auto packet         = GP_CLI_COMMAND_BATTLEFIELD_REQ{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BATTLEFIELD_REQ);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = kind;
    std::memcpy(packet.padding00, padding.data(), padding.size());
    return packet;
}

auto makeSitChairPacket(std::uint32_t mode, std::uint32_t chairId) -> GP_CLI_COMMAND_SITCHAIR
{
    auto packet         = GP_CLI_COMMAND_SITCHAIR{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SITCHAIR);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Mode        = mode;
    packet.ChairId     = chairId;
    return packet;
}

auto validateBattlefieldKindPure(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_BATTLEFIELD_REQ_KIND>(kind);
    return validator;
}

auto validateSitChairScalarsPure(std::uint32_t mode, std::uint32_t chairId) -> PacketValidationResult
{
    // The full SITCHAIR validator starts with live CCharEntity blocked-state checks.
    // This helper pins the portable scalar subset after those checks.
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_SITCHAIR_MODE>(mode)
        .range("ChairId", chairId, 0, 20);
    return validator;
}

auto testBattlefieldSitChairLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_BATTLEFIELD_REQ::name, "GP_CLI_COMMAND_BATTLEFIELD_REQ", "BATTLEFIELD_REQ name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_SITCHAIR::name, "GP_CLI_COMMAND_SITCHAIR", "SITCHAIR name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BATTLEFIELD_REQ::packetId), 0x112, "BATTLEFIELD_REQ packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SITCHAIR::packetId), 0x113, "SITCHAIR packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_BATTLEFIELD_REQ), 8, "BATTLEFIELD_REQ sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_SITCHAIR), 12, "SITCHAIR sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BATTLEFIELD_REQ, Kind), 4, "BATTLEFIELD_REQ Kind offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BATTLEFIELD_REQ, padding00), 5, "BATTLEFIELD_REQ padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_SITCHAIR, Mode), 4, "SITCHAIR Mode offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_SITCHAIR, ChairId), 8, "SITCHAIR ChairId offset") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_BATTLEFIELD_REQ_KIND::Both), 0, "BATTLEFIELD_REQ Both") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_BATTLEFIELD_REQ_KIND::Sidebar), 1, "BATTLEFIELD_REQ Sidebar") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_BATTLEFIELD_REQ_KIND::MapOverlay), 2, "BATTLEFIELD_REQ MapOverlay") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SITCHAIR_MODE::Toggle), 0, "SITCHAIR Toggle") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SITCHAIR_MODE::On), 1, "SITCHAIR On") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SITCHAIR_MODE::Off), 2, "SITCHAIR Off") && ok;
    return ok;
}

auto testBattlefieldSitChairEncodedBytes() -> bool
{
    bool ok = true;

    auto battlefieldExpected = BattlefieldReqBytes{ 0x12, 0x05, 0xEF, 0xBE, 0x02, 0xAA, 0xBB, 0xCC };
    ok                       = expectBytes(encodedPacketBytes(makeBattlefieldReqPacket(2, { 0xAA, 0xBB, 0xCC })), battlefieldExpected, "BATTLEFIELD_REQ encoded packet") && ok;

    auto sitChairExpected = SitChairBytes{ 0x13, 0x07, 0xEF, 0xBE, 0x04, 0x03, 0x02, 0x01, 0xDD, 0xCC, 0xBB, 0xAA };
    ok                    = expectBytes(encodedPacketBytes(makeSitChairPacket(0x01020304, 0xAABBCCDD)), sitChairExpected, "SITCHAIR encoded packet") && ok;

    return ok;
}

auto testBattlefieldSitChairValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateBattlefieldKindPure(0), "BATTLEFIELD_REQ kind min validation") && ok;
    ok      = expectValid(validateBattlefieldKindPure(2), "BATTLEFIELD_REQ kind max validation") && ok;
    ok      = expectInvalidError(validateBattlefieldKindPure(3), "3 not a valid GP_CLI_COMMAND_BATTLEFIELD_REQ_KIND value.", "BATTLEFIELD_REQ invalid kind validation") && ok;
    ok      = expectValid(validateSitChairScalarsPure(0, 0), "SITCHAIR scalar min validation") && ok;
    ok      = expectValid(validateSitChairScalarsPure(2, 20), "SITCHAIR scalar max validation") && ok;
    ok      = expectValid(validateSitChairScalarsPure(0x10002, 20), "SITCHAIR narrowed mode validation") && ok;
    ok      = expectInvalidError(validateSitChairScalarsPure(0x10003, 0), "3 not a valid GP_CLI_COMMAND_SITCHAIR_MODE value.", "SITCHAIR invalid mode validation") && ok;
    ok      = expectInvalidError(validateSitChairScalarsPure(0, 21), "ChairId out of range: 21 not in [0, 20]", "SITCHAIR invalid ChairId validation") && ok;
    ok      = expectInvalidError(validateSitChairScalarsPure(3, 21), "3 not a valid GP_CLI_COMMAND_SITCHAIR_MODE value.", "SITCHAIR validation order") && ok;
    return ok;
}

} // namespace

auto runC2SBattlefieldSitChairPacketSelfTests() -> bool
{
    return testBattlefieldSitChairLayoutsAndMetadata() &&
           testBattlefieldSitChairEncodedBytes() &&
           testBattlefieldSitChairValidation();
}
