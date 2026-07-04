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

#include "test_c2s_pbx_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include "map/entities/char_entity.h"
#include "map/packets/c2s/0x04d_pbx.h"

namespace
{

constexpr auto pbxCommandOffset    = 4U;
constexpr auto pbxBoxNoOffset      = 5U;
constexpr auto pbxPostWorkNoOffset = 6U;
constexpr auto pbxItemWorkNoOffset = 7U;
constexpr auto pbxItemStacksOffset = 8U;
constexpr auto pbxResultOffset     = 12U;
constexpr auto pbxResParam1Offset  = 13U;
constexpr auto pbxResParam2Offset  = 14U;
constexpr auto pbxResParam3Offset  = 15U;
constexpr auto pbxTargetNameOffset = 16U;
constexpr auto pbxTargetNameLen    = 16U;

using PBXBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_PBX)>;

struct PBXValidationInput
{
    std::uint8_t Command;
    std::int8_t  BoxNo;
    std::int8_t  PostWorkNo;
    std::int8_t  ItemWorkNo;
    std::int32_t ItemStacks;
    std::int8_t  Result;
    std::int8_t  ResParam1;
    std::int8_t  ResParam2;
    std::int8_t  ResParam3;
};

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s PBX packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s PBX packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s PBX packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s PBX packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s PBX packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s PBX packet self-test failed: " << label << " got";
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

auto makePBXPacket() -> GP_CLI_COMMAND_PBX
{
    auto packet         = GP_CLI_COMMAND_PBX{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_PBX);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Command     = static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Set);
    packet.BoxNo       = static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Outgoing);
    packet.PostWorkNo  = 7;
    packet.ItemWorkNo  = -2;
    packet.ItemStacks  = 123456789;
    packet.Result      = -3;
    packet.ResParam1   = -4;
    packet.ResParam2   = -5;
    packet.ResParam3   = -6;

    const auto name = std::array<std::uint8_t, pbxTargetNameLen>{
        'R',
        'e',
        'c',
        'i',
        'p',
        'i',
        'e',
        'n',
        't',
        0x00,
        0xFE,
        0xFD,
        '1',
        '2',
        '3',
        0x80,
    };
    std::memcpy(packet.TargetName, name.data(), name.size());
    return packet;
}

auto makeExpectedPBXBytes() -> PBXBytes
{
    auto bytes = PBXBytes{};
    bytes[0] = 0x4D;
    bytes[1] = 0x10;
    bytes[2] = 0xEF;
    bytes[3] = 0xBE;
    bytes[pbxCommandOffset] = static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Set);
    bytes[pbxBoxNoOffset] = static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_BOXNO::Outgoing);
    bytes[pbxPostWorkNoOffset] = 0x07;
    bytes[pbxItemWorkNoOffset] = 0xFE;
    bytes[pbxItemStacksOffset] = 0x15;
    bytes[pbxItemStacksOffset + 1U] = 0xCD;
    bytes[pbxItemStacksOffset + 2U] = 0x5B;
    bytes[pbxItemStacksOffset + 3U] = 0x07;
    bytes[pbxResultOffset] = 0xFD;
    bytes[pbxResParam1Offset] = 0xFC;
    bytes[pbxResParam2Offset] = 0xFB;
    bytes[pbxResParam3Offset] = 0xFA;
    const auto name = std::array<std::uint8_t, pbxTargetNameLen>{
        'R',
        'e',
        'c',
        'i',
        'p',
        'i',
        'e',
        'n',
        't',
        0x00,
        0xFE,
        0xFD,
        '1',
        '2',
        '3',
        0x80,
    };
    std::memcpy(bytes.data() + pbxTargetNameOffset, name.data(), name.size());
    return bytes;
}

auto validatePBXPacket(const PBXValidationInput& input) -> PacketValidationResult
{
    auto packet        = GP_CLI_COMMAND_PBX{};
    packet.Command    = input.Command;
    packet.BoxNo      = input.BoxNo;
    packet.PostWorkNo = input.PostWorkNo;
    packet.ItemWorkNo = input.ItemWorkNo;
    packet.ItemStacks = input.ItemStacks;
    packet.Result     = input.Result;
    packet.ResParam1  = input.ResParam1;
    packet.ResParam2  = input.ResParam2;
    packet.ResParam3  = input.ResParam3;

    auto character = CCharEntity{};
    return packet.validate(nullptr, &character);
}

auto validPBXWork(std::int8_t boxNo, std::int8_t postWorkNo) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Work),
        boxNo,
        postWorkNo,
        -1,
        -1,
        0,
        0,
        0,
        0,
    };
}

auto validPBXSet(std::int8_t postWorkNo, std::int32_t itemStacks) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Set),
        static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Outgoing),
        postWorkNo,
        7,
        itemStacks,
        0,
        0,
        0,
        0,
    };
}

auto validPBXOutgoingCell(GP_CLI_COMMAND_PBX_COMMAND command, std::int8_t postWorkNo) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(command),
        static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Outgoing),
        postWorkNo,
        -1,
        -1,
        0,
        0,
        0,
        0,
    };
}

auto validPBXCheck(std::int8_t boxNo) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Check),
        boxNo,
        -1,
        -1,
        -1,
        0,
        0,
        0,
        0,
    };
}

auto validPBXRecv(std::int8_t postWorkNo) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Recv),
        static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming),
        postWorkNo,
        1,
        -1,
        0,
        0,
        0,
        0,
    };
}

auto validPBXNoneCommand(GP_CLI_COMMAND_PBX_COMMAND command) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(command),
        static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::None),
        -1,
        -1,
        -1,
        0,
        0,
        0,
        0,
    };
}

auto validPBXIncomingCell(GP_CLI_COMMAND_PBX_COMMAND command, std::int8_t postWorkNo) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(command),
        static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming),
        postWorkNo,
        -1,
        -1,
        0,
        0,
        0,
        0,
    };
}

auto validPBXBoxCell(GP_CLI_COMMAND_PBX_COMMAND command, std::int8_t boxNo, std::int8_t postWorkNo) -> PBXValidationInput
{
    return {
        static_cast<std::uint8_t>(command),
        boxNo,
        postWorkNo,
        -1,
        -1,
        0,
        0,
        0,
        0,
    };
}

auto testPBXLayoutAndMetadata() -> bool
{
    bool ok = true;
    const auto packet = GP_CLI_COMMAND_PBX{};

    ok = expectEqualString(GP_CLI_COMMAND_PBX::name, "GP_CLI_COMMAND_PBX", "PBX name") && ok;
    ok = expectEqualUInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_PBX::packetId), 0x04D, "PBX packet id") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_HEADER), 4, "client header size") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_COMMAND_PBX), 32, "PBX sizeof") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, Command), pbxCommandOffset, "Command offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, BoxNo), pbxBoxNoOffset, "BoxNo offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, PostWorkNo), pbxPostWorkNoOffset, "PostWorkNo offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, ItemWorkNo), pbxItemWorkNoOffset, "ItemWorkNo offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, ItemStacks), pbxItemStacksOffset, "ItemStacks offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, Result), pbxResultOffset, "Result offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, ResParam1), pbxResParam1Offset, "ResParam1 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, ResParam2), pbxResParam2Offset, "ResParam2 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, ResParam3), pbxResParam3Offset, "ResParam3 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_PBX, TargetName), pbxTargetNameOffset, "TargetName offset") && ok;
    ok = expectEqualUInt(sizeof(packet.TargetName), pbxTargetNameLen, "TargetName len") && ok;

    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Work), 0x01, "Work") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::Set), 0x02, "Set") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_PBX_COMMAND::PostClose), 0x0F, "PostClose") && ok;
    ok = expectEqualInt(static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::None), -1, "BoxNo None") && ok;
    ok = expectEqualInt(static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming), 1, "BoxNo Incoming") && ok;
    ok = expectEqualInt(static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Outgoing), 2, "BoxNo Outgoing") && ok;

    return ok;
}

auto testPBXEncodedBytes() -> bool
{
    return expectBytes(encodedPacketBytes(makePBXPacket()), makeExpectedPBXBytes(), "PBX bytes");
}

auto testPBXValidation() -> bool
{
    bool ok = true;

    ok = expectValid(validatePBXPacket(validPBXWork(static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming), -1)), "work incoming") && ok;
    ok = expectValid(validatePBXPacket(validPBXWork(static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Outgoing), 8)), "work outgoing") && ok;
    ok = expectValid(validatePBXPacket(validPBXSet(0, 0)), "set min") && ok;
    ok = expectValid(validatePBXPacket(validPBXSet(8, 999999999)), "set max") && ok;
    ok = expectValid(validatePBXPacket(validPBXOutgoingCell(GP_CLI_COMMAND_PBX_COMMAND::Send, 0)), "send") && ok;
    ok = expectValid(validatePBXPacket(validPBXOutgoingCell(GP_CLI_COMMAND_PBX_COMMAND::Cancel, 8)), "cancel") && ok;
    ok = expectValid(validatePBXPacket(validPBXCheck(static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming))), "check incoming") && ok;
    ok = expectValid(validatePBXPacket(validPBXRecv(8)), "recv") && ok;
    ok = expectValid(validatePBXPacket(validPBXNoneCommand(GP_CLI_COMMAND_PBX_COMMAND::Confirm)), "confirm") && ok;
    ok = expectValid(validatePBXPacket(validPBXIncomingCell(GP_CLI_COMMAND_PBX_COMMAND::Accept, 0)), "accept") && ok;
    ok = expectValid(validatePBXPacket(validPBXIncomingCell(GP_CLI_COMMAND_PBX_COMMAND::Reject, 8)), "reject") && ok;
    ok = expectValid(validatePBXPacket(validPBXBoxCell(GP_CLI_COMMAND_PBX_COMMAND::Get, static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming), 0)), "get") && ok;
    ok = expectValid(validatePBXPacket(validPBXBoxCell(GP_CLI_COMMAND_PBX_COMMAND::Clear, static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Outgoing), 8)), "clear") && ok;
    ok = expectValid(validatePBXPacket(validPBXNoneCommand(GP_CLI_COMMAND_PBX_COMMAND::Query)), "query") && ok;
    ok = expectValid(validatePBXPacket(validPBXNoneCommand(GP_CLI_COMMAND_PBX_COMMAND::DeliOpen)), "deli open") && ok;
    ok = expectValid(validatePBXPacket(validPBXNoneCommand(GP_CLI_COMMAND_PBX_COMMAND::PostOpen)), "post open") && ok;
    ok = expectValid(validatePBXPacket(validPBXNoneCommand(GP_CLI_COMMAND_PBX_COMMAND::PostClose)), "post close") && ok;

    auto invalid = validPBXSet(0, 0);
    invalid.Command = 0;
    ok = expectInvalidError(validatePBXPacket(invalid), "0 not a valid GP_CLI_COMMAND_PBX_COMMAND value.", "invalid command") && ok;

    invalid = validPBXSet(0, 0);
    invalid.Result = 1;
    ok = expectInvalidError(validatePBXPacket(invalid), "Result not 0", "result") && ok;

    invalid = validPBXWork(static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming), 0);
    invalid.BoxNo = 0;
    ok = expectInvalidError(validatePBXPacket(invalid), "BoxNo out of range: 0 not in [1, 2]", "work box") && ok;

    invalid = validPBXSet(0, 0);
    invalid.BoxNo = static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming);
    ok = expectInvalidError(validatePBXPacket(invalid), "BoxNo not Outgoing", "set box") && ok;

    invalid = validPBXSet(0, 1000000000);
    ok = expectInvalidError(validatePBXPacket(invalid), "ItemStacks out of range: 1000000000 not in [0, 999999999]", "set stacks") && ok;

    invalid = validPBXRecv(0);
    invalid.ItemWorkNo = -1;
    ok = expectInvalidError(validatePBXPacket(invalid), "ItemWorkNo not 1", "recv item work") && ok;

    invalid = validPBXNoneCommand(GP_CLI_COMMAND_PBX_COMMAND::Confirm);
    invalid.BoxNo = static_cast<std::int8_t>(GP_CLI_COMMAND_PBX_BOXNO::Incoming);
    ok = expectInvalidError(validatePBXPacket(invalid), "BoxNo not None", "confirm box") && ok;

    return ok;
}

} // namespace

auto runC2SPBXPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testPBXLayoutAndMetadata() && ok;
    ok      = testPBXEncodedBytes() && ok;
    ok      = testPBXValidation() && ok;
    return ok;
}
