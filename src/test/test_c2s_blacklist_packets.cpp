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

#include "test_c2s_blacklist_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

#include "map/packets/c2s/0x03c_black_list.h"
#include "map/packets/c2s/0x03d_black_edit.h"

namespace
{

using BlackListPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BLACK_LIST)>;
using BlackEditPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BLACK_EDIT)>;

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s BLACK_LIST/BLACK_EDIT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s BLACK_LIST/BLACK_EDIT packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s BLACK_LIST/BLACK_EDIT packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s BLACK_LIST/BLACK_EDIT packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s BLACK_LIST/BLACK_EDIT packet self-test failed: " << label << " got";
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

template <typename E>
auto enumValue(E value) -> std::int64_t
{
    return static_cast<std::int64_t>(static_cast<std::underlying_type_t<E>>(value));
}

auto expectValid(const PacketValidationResult& result, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectTrue(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), "", label + " error string") && ok;
    return ok;
}

auto expectInvalid(const PacketValidationResult& result, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " valid") && ok;
    ok      = expectFalse(result.errorString().empty(), label + " error string empty") && ok;
    return ok;
}

auto editNameBytes() -> std::array<std::uint8_t, 16>
{
    return {
        0x41,
        0x6C,
        0x69,
        0x63,
        0x65,
        0x00,
        0x58,
        0xFF,
        0x42,
        0x6C,
        0x61,
        0x63,
        0x6B,
        0x80,
        0x01,
        0x02,
    };
}

auto encodedBlackListPacket() -> BlackListPacketBytes
{
    auto packet         = GP_CLI_COMMAND_BLACK_LIST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BLACK_LIST);
    packet.header.size = sizeof(GP_CLI_COMMAND_BLACK_LIST) / 4;
    packet.header.sync = 0xBEEF;
    packet.unknown00   = 0x11223344;
    packet.unknown01   = 0x55667788;
    packet.unknown02   = 0x99AABBCC;
    packet.unknown03   = 0xDDEEFF00;
    packet.unknown04   = 0x12345678;
    packet.unknown05   = 0x9A;
    packet.padding00[0] = 0xBC;
    packet.padding00[1] = 0xDE;
    packet.padding00[2] = 0xF0;

    auto bytes = BlackListPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedBlackEditPacket() -> BlackEditPacketBytes
{
    auto packet         = GP_CLI_COMMAND_BLACK_EDIT{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BLACK_EDIT);
    packet.header.size = sizeof(GP_CLI_COMMAND_BLACK_EDIT) / 4;
    packet.header.sync = 0xBEEF;
    packet.Data.ID     = 0x11223344;

    const auto name = editNameBytes();
    for (std::size_t i = 0; i < name.size(); ++i)
    {
        packet.Data.Name[i] = name[i];
    }

    packet.Mode         = static_cast<std::int8_t>(enumValue(GP_CLI_COMMAND_BLACK_EDIT_MODE::Remove));
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;

    auto bytes = BlackEditPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto blackListNoPureScalarValidation() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return static_cast<PacketValidationResult>(validator);
}

auto blackEditPureValidation(std::int8_t mode) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_BLACK_EDIT_MODE>(mode);
    return static_cast<PacketValidationResult>(validator);
}

auto testBlackListLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_BLACK_LIST::name, "GP_CLI_COMMAND_BLACK_LIST", "BLACK_LIST static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_BLACK_LIST::packetId == PacketC2S::GP_CLI_COMMAND_BLACK_LIST, "BLACK_LIST static packetId") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(sizeof(GP_CLI_HEADER)), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(sizeof(GP_CLI_COMMAND_BLACK_LIST)), 28, "sizeof(GP_CLI_COMMAND_BLACK_LIST)") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, header)), 0, "BLACK_LIST header offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, unknown00)), 4, "BLACK_LIST unknown00 offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, unknown01)), 8, "BLACK_LIST unknown01 offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, unknown02)), 12, "BLACK_LIST unknown02 offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, unknown03)), 16, "BLACK_LIST unknown03 offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, unknown04)), 20, "BLACK_LIST unknown04 offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, unknown05)), 24, "BLACK_LIST unknown05 offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_LIST, padding00)), 25, "BLACK_LIST padding00 offset") && ok;
    ok = expectBytes(encodedBlackListPacket(),
                     BlackListPacketBytes{ 0x3C, 0x0E, 0xEF, 0xBE,
                                           0x44, 0x33, 0x22, 0x11,
                                           0x88, 0x77, 0x66, 0x55,
                                           0xCC, 0xBB, 0xAA, 0x99,
                                           0x00, 0xFF, 0xEE, 0xDD,
                                           0x78, 0x56, 0x34, 0x12,
                                           0x9A, 0xBC, 0xDE, 0xF0 },
                     "BLACK_LIST encoded packet") &&
         ok;

    return ok;
}

auto testBlackListPayloadStorage() -> bool
{
    auto packet         = GP_CLI_COMMAND_BLACK_LIST{};
    packet.unknown00   = 0x11223344;
    packet.unknown01   = 0x55667788;
    packet.unknown02   = 0x99AABBCC;
    packet.unknown03   = 0xDDEEFF00;
    packet.unknown04   = 0x12345678;
    packet.unknown05   = 0x9A;
    packet.padding00[0] = 0xBC;
    packet.padding00[1] = 0xDE;
    packet.padding00[2] = 0xF0;

    bool ok = true;
    ok      = expectEqualInt(packet.unknown00, 0x11223344, "BLACK_LIST unknown00") && ok;
    ok      = expectEqualInt(packet.unknown01, 0x55667788, "BLACK_LIST unknown01") && ok;
    ok      = expectEqualInt(packet.unknown02, 0x99AABBCC, "BLACK_LIST unknown02") && ok;
    ok      = expectEqualInt(packet.unknown03, 0xDDEEFF00, "BLACK_LIST unknown03") && ok;
    ok      = expectEqualInt(packet.unknown04, 0x12345678, "BLACK_LIST unknown04") && ok;
    ok      = expectEqualInt(packet.unknown05, 0x9A, "BLACK_LIST unknown05") && ok;
    ok      = expectEqualInt(packet.padding00[0], 0xBC, "BLACK_LIST padding00[0]") && ok;
    ok      = expectEqualInt(packet.padding00[1], 0xDE, "BLACK_LIST padding00[1]") && ok;
    ok      = expectEqualInt(packet.padding00[2], 0xF0, "BLACK_LIST padding00[2]") && ok;
    return ok;
}

auto testBlackListPortableValidationFacts() -> bool
{
    return expectValid(blackListNoPureScalarValidation(), "BLACK_LIST no pure scalar validation");
}

auto testBlackEditLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_BLACK_EDIT::name, "GP_CLI_COMMAND_BLACK_EDIT", "BLACK_EDIT static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_BLACK_EDIT::packetId == PacketC2S::GP_CLI_COMMAND_BLACK_EDIT, "BLACK_EDIT static packetId") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(sizeof(SAVE_BLACK)), 20, "sizeof(SAVE_BLACK)") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(SAVE_BLACK, ID)), 0, "SAVE_BLACK ID offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(SAVE_BLACK, Name)), 4, "SAVE_BLACK Name offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(sizeof(GP_CLI_COMMAND_BLACK_EDIT)), 28, "sizeof(GP_CLI_COMMAND_BLACK_EDIT)") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_EDIT, header)), 0, "BLACK_EDIT header offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_EDIT, Data)), 4, "BLACK_EDIT Data offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_EDIT, Mode)), 24, "BLACK_EDIT Mode offset") && ok;
    ok = expectEqualInt(static_cast<std::int64_t>(offsetof(GP_CLI_COMMAND_BLACK_EDIT, padding00)), 25, "BLACK_EDIT padding00 offset") && ok;
    ok = expectBytes(encodedBlackEditPacket(),
                     BlackEditPacketBytes{ 0x3D, 0x0E, 0xEF, 0xBE,
                                           0x44, 0x33, 0x22, 0x11,
                                           0x41, 0x6C, 0x69, 0x63,
                                           0x65, 0x00, 0x58, 0xFF,
                                           0x42, 0x6C, 0x61, 0x63,
                                           0x6B, 0x80, 0x01, 0x02,
                                           0x01, 0xAA, 0xBB, 0xCC },
                     "BLACK_EDIT encoded packet") &&
         ok;

    return ok;
}

auto testBlackEditEnumValuesAndDomains() -> bool
{
    bool ok = true;

    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_BLACK_EDIT_MODE::Add), 0, "Mode::Add") && ok;
    ok = expectEqualInt(enumValue(GP_CLI_COMMAND_BLACK_EDIT_MODE::Remove), 1, "Mode::Remove") && ok;
    ok = expectTrue(magic_enum::enum_contains<GP_CLI_COMMAND_BLACK_EDIT_MODE>(static_cast<std::int8_t>(0)), "Mode domain contains 0") && ok;
    ok = expectTrue(magic_enum::enum_contains<GP_CLI_COMMAND_BLACK_EDIT_MODE>(static_cast<std::int8_t>(1)), "Mode domain contains 1") && ok;
    ok = expectFalse(magic_enum::enum_contains<GP_CLI_COMMAND_BLACK_EDIT_MODE>(static_cast<std::int8_t>(-1)), "Mode domain rejects -1") && ok;
    ok = expectFalse(magic_enum::enum_contains<GP_CLI_COMMAND_BLACK_EDIT_MODE>(static_cast<std::int8_t>(2)), "Mode domain rejects 2") && ok;

    return ok;
}

auto testBlackEditPayloadStorage() -> bool
{
    auto packet     = GP_CLI_COMMAND_BLACK_EDIT{};
    packet.Data.ID = 0x55667788;

    const auto name = editNameBytes();
    for (std::size_t i = 0; i < name.size(); ++i)
    {
        packet.Data.Name[i] = name[i];
    }

    packet.Mode         = -2;
    packet.padding00[0] = 0x11;
    packet.padding00[1] = 0x22;
    packet.padding00[2] = 0x33;

    bool ok = true;
    ok      = expectEqualInt(packet.Data.ID, 0x55667788, "BLACK_EDIT Data.ID") && ok;
    for (std::size_t i = 0; i < name.size(); ++i)
    {
        ok = expectEqualInt(packet.Data.Name[i], name[i], "BLACK_EDIT Data.Name[" + std::to_string(i) + "]") && ok;
    }
    ok = expectEqualInt(packet.Mode, -2, "BLACK_EDIT signed Mode") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(packet.Mode), 0xFE, "BLACK_EDIT raw Mode byte") && ok;
    ok = expectEqualInt(packet.padding00[0], 0x11, "BLACK_EDIT padding00[0]") && ok;
    ok = expectEqualInt(packet.padding00[1], 0x22, "BLACK_EDIT padding00[1]") && ok;
    ok = expectEqualInt(packet.padding00[2], 0x33, "BLACK_EDIT padding00[2]") && ok;
    return ok;
}

auto testBlackEditPortableValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(blackEditPureValidation(static_cast<std::int8_t>(0)), "BLACK_EDIT add validation") && ok;
    ok      = expectValid(blackEditPureValidation(static_cast<std::int8_t>(1)), "BLACK_EDIT remove validation") && ok;
    ok      = expectInvalid(blackEditPureValidation(static_cast<std::int8_t>(-1)), "BLACK_EDIT negative Mode validation") && ok;
    ok      = expectInvalid(blackEditPureValidation(static_cast<std::int8_t>(2)), "BLACK_EDIT invalid Mode validation") && ok;
    return ok;
}

} // namespace

auto runC2SBlacklistPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testBlackListLayoutAndMetadata() && ok;
    ok = testBlackListPayloadStorage() && ok;
    ok = testBlackListPortableValidationFacts() && ok;
    ok = testBlackEditLayoutAndMetadata() && ok;
    ok = testBlackEditEnumValuesAndDomains() && ok;
    ok = testBlackEditPayloadStorage() && ok;
    ok = testBlackEditPortableValidationFacts() && ok;

    return ok;
}
