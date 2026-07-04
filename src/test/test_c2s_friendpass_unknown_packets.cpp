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

#include "test_c2s_friendpass_unknown_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x01b_friendpass.h"
#include "map/packets/c2s/0x01c_unknown.h"

namespace
{

using FriendPassBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_FRIENDPASS)>;
using UnknownBytes    = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_UNKNOWN)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s FRIENDPASS/UNKNOWN packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s FRIENDPASS/UNKNOWN packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s FRIENDPASS/UNKNOWN packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s FRIENDPASS/UNKNOWN packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s FRIENDPASS/UNKNOWN packet self-test failed: " << label << " got";
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

auto encodedFriendPassPacket() -> FriendPassBytes
{
    auto packet          = GP_CLI_COMMAND_FRIENDPASS{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_FRIENDPASS);
    packet.header.size  = sizeof(GP_CLI_COMMAND_FRIENDPASS) / 4;
    packet.header.sync  = 0xBEEF;
    packet.Para         = static_cast<std::uint16_t>(GP_CLI_COMMAND_FRIENDPASS_PARA::ConfirmGoldPurchase);
    packet.padding00    = 0x55AA;

    auto bytes = FriendPassBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedUnknownPacket() -> UnknownBytes
{
    auto packet          = GP_CLI_COMMAND_UNKNOWN{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_UNKNOWN);
    packet.header.size  = sizeof(GP_CLI_COMMAND_UNKNOWN) / 4;
    packet.header.sync  = 0xBEEF;
    packet.unknown00    = 0x1122;
    packet.padding00    = 0x3344;
    packet.unknown01    = 0x55667788;

    auto bytes = UnknownBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto validateFriendPassPara(std::uint16_t para) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_FRIENDPASS_PARA>(para);
    return validator;
}

auto testFriendPassLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_FRIENDPASS::name, "GP_CLI_COMMAND_FRIENDPASS", "FRIENDPASS static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_FRIENDPASS::packetId == PacketC2S::GP_CLI_COMMAND_FRIENDPASS, "FRIENDPASS static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_FRIENDPASS), 8, "sizeof(GP_CLI_COMMAND_FRIENDPASS)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRIENDPASS, header), 0, "FRIENDPASS header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRIENDPASS, Para), 4, "Para offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRIENDPASS, padding00), 6, "FRIENDPASS padding00 offset") && ok;
    ok = expectBytes(encodedFriendPassPacket(),
                     FriendPassBytes{ 0x1B, 0x04, 0xEF, 0xBE,
                                      0x03, 0x00, 0xAA, 0x55 },
                     "FRIENDPASS encoded packet") &&
         ok;

    return ok;
}

auto testFriendPassEnumValuesAndDomain() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FRIENDPASS_PARA::BeginPurchase), 0, "BeginPurchase enum") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FRIENDPASS_PARA::ConfirmPurchase), 1, "ConfirmPurchase enum") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FRIENDPASS_PARA::BeginGoldPurchase), 2, "BeginGoldPurchase enum") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FRIENDPASS_PARA::ConfirmGoldPurchase), 3, "ConfirmGoldPurchase enum") && ok;
    ok = expectValid(validateFriendPassPara(0), "BeginPurchase validation") && ok;
    ok = expectValid(validateFriendPassPara(1), "ConfirmPurchase validation") && ok;
    ok = expectValid(validateFriendPassPara(2), "BeginGoldPurchase validation") && ok;
    ok = expectValid(validateFriendPassPara(3), "ConfirmGoldPurchase validation") && ok;
    ok = expectInvalidError(validateFriendPassPara(4), "4 not a valid GP_CLI_COMMAND_FRIENDPASS_PARA value.", "invalid Para validation") && ok;
    ok = expectInvalidError(validateFriendPassPara(0xFFFF), "65535 not a valid GP_CLI_COMMAND_FRIENDPASS_PARA value.", "max Para validation") && ok;

    return ok;
}

auto testUnknownLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_UNKNOWN::name, "GP_CLI_COMMAND_UNKNOWN", "UNKNOWN static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_UNKNOWN::packetId == PacketC2S::GP_CLI_COMMAND_UNKNOWN, "UNKNOWN static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_UNKNOWN), 12, "sizeof(GP_CLI_COMMAND_UNKNOWN)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_UNKNOWN, header), 0, "UNKNOWN header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_UNKNOWN, unknown00), 4, "unknown00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_UNKNOWN, padding00), 6, "UNKNOWN padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_UNKNOWN, unknown01), 8, "unknown01 offset") && ok;
    ok = expectBytes(encodedUnknownPacket(),
                     UnknownBytes{ 0x1C, 0x06, 0xEF, 0xBE,
                                   0x22, 0x11, 0x44, 0x33,
                                   0x88, 0x77, 0x66, 0x55 },
                     "UNKNOWN encoded packet") &&
         ok;

    return ok;
}

} // namespace

auto runC2SFriendPassUnknownPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testFriendPassLayoutAndMetadata() && ok;
    ok = testFriendPassEnumValuesAndDomain() && ok;
    ok = testUnknownLayoutAndMetadata() && ok;

    return ok;
}
