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

#include "test_c2s_unity_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x116_unity_menu.h"
#include "map/packets/c2s/0x117_unity_quest.h"
#include "map/packets/c2s/0x118_unity_toggle.h"

namespace
{

using UnityMenuBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_UNITY_MENU)>;
using UnityQuestBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_UNITY_QUEST)>;
using UnityToggleBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_UNITY_TOGGLE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s unity packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s unity packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s unity packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s unity packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s unity packet self-test failed: " << label << " got";
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

auto makeUnityMenuPacket(std::uint32_t kind) -> GP_CLI_COMMAND_UNITY_MENU
{
    auto packet         = GP_CLI_COMMAND_UNITY_MENU{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_UNITY_MENU);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = kind;
    return packet;
}

auto makeUnityQuestPacket(std::uint32_t kind) -> GP_CLI_COMMAND_UNITY_QUEST
{
    auto packet         = GP_CLI_COMMAND_UNITY_QUEST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_UNITY_QUEST);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = kind;
    return packet;
}

auto makeUnityTogglePacket(std::uint8_t mode, std::array<std::uint8_t, 3> padding) -> GP_CLI_COMMAND_UNITY_TOGGLE
{
    auto packet         = GP_CLI_COMMAND_UNITY_TOGGLE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_UNITY_TOGGLE);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Mode        = mode;
    std::memcpy(packet.padding00, padding.data(), padding.size());
    return packet;
}

auto validateUnityMenuKindPure(std::uint32_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("Kind", kind, 0x0, 0x1);
    return validator;
}

auto validateUnityQuestKindPure(std::uint32_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(kind, 0x0, "Kind not 0x0");
    return validator;
}

auto validateUnityToggleModePure(std::uint8_t mode) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_UNITY_TOGGLE_MODE>(mode);
    return validator;
}

auto testUnityLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_UNITY_MENU::name, "GP_CLI_COMMAND_UNITY_MENU", "UNITY_MENU name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_UNITY_QUEST::name, "GP_CLI_COMMAND_UNITY_QUEST", "UNITY_QUEST name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_UNITY_TOGGLE::name, "GP_CLI_COMMAND_UNITY_TOGGLE", "UNITY_TOGGLE name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_UNITY_MENU::packetId), 0x116, "UNITY_MENU packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_UNITY_QUEST::packetId), 0x117, "UNITY_QUEST packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_UNITY_TOGGLE::packetId), 0x118, "UNITY_TOGGLE packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_UNITY_MENU), 8, "UNITY_MENU sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_UNITY_QUEST), 8, "UNITY_QUEST sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_UNITY_TOGGLE), 8, "UNITY_TOGGLE sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_UNITY_MENU, Kind), 4, "UNITY_MENU Kind offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_UNITY_QUEST, Kind), 4, "UNITY_QUEST Kind offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_UNITY_TOGGLE, Mode), 4, "UNITY_TOGGLE Mode offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_UNITY_TOGGLE, padding00), 5, "UNITY_TOGGLE padding00 offset") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_UNITY_TOGGLE_MODE::Inactive), 0, "UNITY_TOGGLE Inactive") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_UNITY_TOGGLE_MODE::Active), 1, "UNITY_TOGGLE Active") && ok;
    return ok;
}

auto testUnityEncodedBytes() -> bool
{
    bool ok = true;

    auto menuExpected = UnityMenuBytes{ 0x16, 0x05, 0xEF, 0xBE, 0x01, 0x00, 0x00, 0x00 };
    ok                = expectBytes(encodedPacketBytes(makeUnityMenuPacket(1)), menuExpected, "UNITY_MENU encoded packet") && ok;

    auto questExpected = UnityQuestBytes{ 0x17, 0x05, 0xEF, 0xBE, 0xDD, 0xCC, 0xBB, 0xAA };
    ok                 = expectBytes(encodedPacketBytes(makeUnityQuestPacket(0xAABBCCDD)), questExpected, "UNITY_QUEST encoded packet") && ok;

    auto toggleExpected = UnityToggleBytes{ 0x18, 0x05, 0xEF, 0xBE, 0x01, 0xAA, 0xBB, 0xCC };
    ok                  = expectBytes(encodedPacketBytes(makeUnityTogglePacket(1, { 0xAA, 0xBB, 0xCC })), toggleExpected, "UNITY_TOGGLE encoded packet") && ok;

    return ok;
}

auto testUnityValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateUnityMenuKindPure(0), "UNITY_MENU Kind min validation") && ok;
    ok      = expectValid(validateUnityMenuKindPure(1), "UNITY_MENU Kind max validation") && ok;
    ok      = expectInvalidError(validateUnityMenuKindPure(2), "Kind out of range: 2 not in [0, 1]", "UNITY_MENU invalid Kind validation") && ok;
    ok      = expectValid(validateUnityQuestKindPure(0), "UNITY_QUEST Kind validation") && ok;
    ok      = expectInvalidError(validateUnityQuestKindPure(1), "Kind not 0x0", "UNITY_QUEST invalid Kind validation") && ok;
    ok      = expectValid(validateUnityToggleModePure(0), "UNITY_TOGGLE inactive validation") && ok;
    ok      = expectValid(validateUnityToggleModePure(1), "UNITY_TOGGLE active validation") && ok;
    ok      = expectInvalidError(validateUnityToggleModePure(2), "2 not a valid GP_CLI_COMMAND_UNITY_TOGGLE_MODE value.", "UNITY_TOGGLE invalid mode validation") && ok;
    return ok;
}

} // namespace

auto runC2SUnityPacketSelfTests() -> bool
{
    return testUnityLayoutsAndMetadata() &&
           testUnityEncodedBytes() &&
           testUnityValidation();
}
