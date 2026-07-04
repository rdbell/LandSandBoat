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

#include "test_c2s_tail_misc_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x119_emote_list.h"
#include "map/packets/c2s/0x11b_mastery_display.h"
#include "map/packets/c2s/0x11c_party_request.h"
#include "map/packets/c2s/0x11d_jump.h"

namespace
{

using EmoteListBytes      = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EMOTE_LIST)>;
using MasteryDisplayBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MASTERY_DISPLAY)>;
using PartyRequestBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_PARTY_REQUEST)>;
using JumpBytes           = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_JUMP)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s tail misc packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s tail misc packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s tail misc packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s tail misc packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s tail misc packet self-test failed: " << label << " got";
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

auto makeEmoteListPacket() -> GP_CLI_COMMAND_EMOTE_LIST
{
    auto packet         = GP_CLI_COMMAND_EMOTE_LIST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EMOTE_LIST);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto makeMasteryDisplayPacket(std::uint8_t mode, std::array<std::uint8_t, 3> padding) -> GP_CLI_COMMAND_MASTERY_DISPLAY
{
    auto packet         = GP_CLI_COMMAND_MASTERY_DISPLAY{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MASTERY_DISPLAY);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Mode        = mode;
    std::memcpy(packet.padding00, padding.data(), padding.size());
    return packet;
}

auto makePartyRequestPacket() -> GP_CLI_COMMAND_PARTY_REQUEST
{
    auto packet         = GP_CLI_COMMAND_PARTY_REQUEST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_PARTY_REQUEST);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x01020304;
    packet.ActIndex    = 0x0506;
    packet.Kind        = 0x01;
    packet.padding00   = 0x07;
    packet.padding01   = 0xAABBCCDD;
    return packet;
}

auto makeJumpPacket() -> GP_CLI_COMMAND_JUMP
{
    auto packet         = GP_CLI_COMMAND_JUMP{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_JUMP);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x01020304;
    packet.ActIndex    = 0x0506;
    packet.padding00   = 0x0708;
    return packet;
}

auto validateMasteryDisplayModePure(std::uint8_t mode) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_MASTERY_DISPLAY_MODE>(mode);
    return validator;
}

auto validatePartyRequestKindPure(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_PARTY_REQUEST_KIND>(kind);
    return validator;
}

auto testTailMiscLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_EMOTE_LIST::name, "GP_CLI_COMMAND_EMOTE_LIST", "EMOTE_LIST name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_MASTERY_DISPLAY::name, "GP_CLI_COMMAND_MASTERY_DISPLAY", "MASTERY_DISPLAY name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_PARTY_REQUEST::name, "GP_CLI_COMMAND_PARTY_REQUEST", "PARTY_REQUEST name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_JUMP::name, "GP_CLI_COMMAND_JUMP", "JUMP name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EMOTE_LIST::packetId), 0x119, "EMOTE_LIST packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MASTERY_DISPLAY::packetId), 0x11B, "MASTERY_DISPLAY packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_PARTY_REQUEST::packetId), 0x11C, "PARTY_REQUEST packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_JUMP::packetId), 0x11D, "JUMP packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_EMOTE_LIST), 4, "EMOTE_LIST sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_MASTERY_DISPLAY), 8, "MASTERY_DISPLAY sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_PARTY_REQUEST), 16, "PARTY_REQUEST sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_JUMP), 12, "JUMP sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MASTERY_DISPLAY, Mode), 4, "MASTERY_DISPLAY Mode offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_MASTERY_DISPLAY, padding00), 5, "MASTERY_DISPLAY padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_PARTY_REQUEST, UniqueNo), 4, "PARTY_REQUEST UniqueNo offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_PARTY_REQUEST, ActIndex), 8, "PARTY_REQUEST ActIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_PARTY_REQUEST, Kind), 10, "PARTY_REQUEST Kind offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_PARTY_REQUEST, padding00), 11, "PARTY_REQUEST padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_PARTY_REQUEST, padding01), 12, "PARTY_REQUEST padding01 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_JUMP, UniqueNo), 4, "JUMP UniqueNo offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_JUMP, ActIndex), 8, "JUMP ActIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_JUMP, padding00), 10, "JUMP padding00 offset") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MASTERY_DISPLAY_MODE::Off), 0, "MASTERY_DISPLAY Off") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MASTERY_DISPLAY_MODE::On), 1, "MASTERY_DISPLAY On") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_PARTY_REQUEST_KIND::Add), 0, "PARTY_REQUEST Add") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_PARTY_REQUEST_KIND::Remove), 1, "PARTY_REQUEST Remove") && ok;
    return ok;
}

auto testTailMiscEncodedBytes() -> bool
{
    bool ok = true;

    auto emoteExpected = EmoteListBytes{ 0x19, 0x03, 0xEF, 0xBE };
    ok                 = expectBytes(encodedPacketBytes(makeEmoteListPacket()), emoteExpected, "EMOTE_LIST encoded packet") && ok;

    auto masteryExpected = MasteryDisplayBytes{ 0x1B, 0x05, 0xEF, 0xBE, 0x01, 0xAA, 0xBB, 0xCC };
    ok                   = expectBytes(encodedPacketBytes(makeMasteryDisplayPacket(1, { 0xAA, 0xBB, 0xCC })), masteryExpected, "MASTERY_DISPLAY encoded packet") && ok;

    auto partyExpected = PartyRequestBytes{ 0x1C, 0x09, 0xEF, 0xBE, 0x04, 0x03, 0x02, 0x01, 0x06, 0x05, 0x01, 0x07, 0xDD, 0xCC, 0xBB, 0xAA };
    ok                 = expectBytes(encodedPacketBytes(makePartyRequestPacket()), partyExpected, "PARTY_REQUEST encoded packet") && ok;

    auto jumpExpected = JumpBytes{ 0x1D, 0x07, 0xEF, 0xBE, 0x04, 0x03, 0x02, 0x01, 0x06, 0x05, 0x08, 0x07 };
    ok                = expectBytes(encodedPacketBytes(makeJumpPacket()), jumpExpected, "JUMP encoded packet") && ok;

    return ok;
}

auto testTailMiscValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateMasteryDisplayModePure(0), "MASTERY_DISPLAY off validation") && ok;
    ok      = expectValid(validateMasteryDisplayModePure(1), "MASTERY_DISPLAY on validation") && ok;
    ok      = expectInvalidError(validateMasteryDisplayModePure(2), "2 not a valid GP_CLI_COMMAND_MASTERY_DISPLAY_MODE value.", "MASTERY_DISPLAY invalid mode validation") && ok;
    ok      = expectValid(validatePartyRequestKindPure(0), "PARTY_REQUEST add validation") && ok;
    ok      = expectValid(validatePartyRequestKindPure(1), "PARTY_REQUEST remove validation") && ok;
    ok      = expectInvalidError(validatePartyRequestKindPure(2), "2 not a valid GP_CLI_COMMAND_PARTY_REQUEST_KIND value.", "PARTY_REQUEST invalid kind validation") && ok;
    return ok;
}

} // namespace

auto runC2STailMiscPacketSelfTests() -> bool
{
    return testTailMiscLayoutsAndMetadata() &&
           testTailMiscEncodedBytes() &&
           testTailMiscValidation();
}
