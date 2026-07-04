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

#include "test_c2s_dungeon_inspect_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0d8_dungeon_param.h"
#include "map/packets/c2s/0x0dd_equip_inspect.h"
#include "map/packets/c2s/0x0de_inspect_message.h"

namespace
{

using DungeonParamBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_DUNGEON_PARAM)>;
using EquipInspectBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EQUIP_INSPECT)>;
using InspectMessageBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_INSPECT_MESSAGE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s dungeon/inspect packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s dungeon/inspect packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s dungeon/inspect packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s dungeon/inspect packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s dungeon/inspect packet self-test failed: " << label << " got";
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

auto validateDungeonParamPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto validateEquipInspectPure(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_EQUIP_INSPECT_KIND>(kind);
    return validator;
}

auto validateInspectMessagePure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto makeDungeonParamPacket() -> GP_CLI_COMMAND_DUNGEON_PARAM
{
    auto packet         = GP_CLI_COMMAND_DUNGEON_PARAM{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_DUNGEON_PARAM);
    packet.header.size = sizeof(GP_CLI_COMMAND_DUNGEON_PARAM) / 4U;
    packet.header.sync = 0xBEEF;
    packet.ActIndex    = 0x1234;
    packet.Param1      = 0x5678;
    packet.Param2      = 0x9A;
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;
    packet.UniqueNo     = 0x11223344;
    for (std::size_t i = 0; i < sizeof(packet.Data); ++i)
    {
        packet.Data[i] = static_cast<std::uint8_t>(i + 1);
    }
    return packet;
}

auto makeEquipInspectPacket() -> GP_CLI_COMMAND_EQUIP_INSPECT
{
    auto packet         = GP_CLI_COMMAND_EQUIP_INSPECT{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EQUIP_INSPECT);
    packet.header.size = sizeof(GP_CLI_COMMAND_EQUIP_INSPECT) / 4U;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.ActIndex    = 0x55667788;
    packet.Kind        = static_cast<std::uint8_t>(GP_CLI_COMMAND_EQUIP_INSPECT_KIND::CheckParam);
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;
    return packet;
}

auto makeInspectMessagePacket() -> GP_CLI_COMMAND_INSPECT_MESSAGE
{
    static constexpr auto text = std::string_view("Bazaar");
    auto                  packet = GP_CLI_COMMAND_INSPECT_MESSAGE{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_INSPECT_MESSAGE);
    packet.header.size   = sizeof(GP_CLI_COMMAND_INSPECT_MESSAGE) / 4U;
    packet.header.sync   = 0xBEEF;
    std::memcpy(packet.sInspectMessage, text.data(), text.size());
    packet.padding00 = 0xEE;
    return packet;
}

auto testDungeonInspectLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_DUNGEON_PARAM::name, "GP_CLI_COMMAND_DUNGEON_PARAM", "DUNGEON_PARAM name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_DUNGEON_PARAM::packetId), 0x0D8, "DUNGEON_PARAM packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_DUNGEON_PARAM), 40, "DUNGEON_PARAM sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DUNGEON_PARAM, ActIndex), 4, "DUNGEON_PARAM ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DUNGEON_PARAM, Param1), 6, "DUNGEON_PARAM Param1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DUNGEON_PARAM, Param2), 8, "DUNGEON_PARAM Param2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DUNGEON_PARAM, padding00), 9, "DUNGEON_PARAM padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_DUNGEON_PARAM::padding00), 3, "DUNGEON_PARAM padding00 bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DUNGEON_PARAM, UniqueNo), 12, "DUNGEON_PARAM UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DUNGEON_PARAM, Data), 16, "DUNGEON_PARAM Data offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_DUNGEON_PARAM::Data), 24, "DUNGEON_PARAM Data bytes") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_EQUIP_INSPECT::name, "GP_CLI_COMMAND_EQUIP_INSPECT", "EQUIP_INSPECT name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EQUIP_INSPECT::packetId), 0x0DD, "EQUIP_INSPECT packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EQUIP_INSPECT), 16, "EQUIP_INSPECT sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIP_INSPECT, UniqueNo), 4, "EQUIP_INSPECT UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIP_INSPECT, ActIndex), 8, "EQUIP_INSPECT ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIP_INSPECT, Kind), 12, "EQUIP_INSPECT Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EQUIP_INSPECT, padding00), 13, "EQUIP_INSPECT padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EQUIP_INSPECT::padding00), 3, "EQUIP_INSPECT padding00 bytes") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_INSPECT_MESSAGE::name, "GP_CLI_COMMAND_INSPECT_MESSAGE", "INSPECT_MESSAGE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_INSPECT_MESSAGE::packetId), 0x0DE, "INSPECT_MESSAGE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_INSPECT_MESSAGE), 128, "INSPECT_MESSAGE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_INSPECT_MESSAGE, sInspectMessage), 4, "INSPECT_MESSAGE sInspectMessage offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_INSPECT_MESSAGE::sInspectMessage), 123, "INSPECT_MESSAGE sInspectMessage bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_INSPECT_MESSAGE, padding00), 127, "INSPECT_MESSAGE padding00 offset") && ok;
    return ok;
}

auto testDungeonInspectEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    const auto dungeonParam = makeDungeonParamPacket();
    ok = expectBytes(encodedPacketBytes(dungeonParam),
                     DungeonParamBytes{ 0xD8, 0x14, 0xEF, 0xBE, 0x34, 0x12, 0x78, 0x56,
                                        0x9A, 0xAA, 0xBB, 0xCC, 0x44, 0x33, 0x22, 0x11,
                                        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                                        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 },
                     "DUNGEON_PARAM encoded packet") &&
         ok;
    ok = expectEqualInt(dungeonParam.ActIndex, 0x1234, "DUNGEON_PARAM ActIndex payload") && ok;
    ok = expectEqualInt(dungeonParam.UniqueNo, 0x11223344, "DUNGEON_PARAM UniqueNo payload") && ok;
    ok = expectEqualInt(dungeonParam.Data[23], 0x18, "DUNGEON_PARAM Data payload") && ok;

    const auto equipInspect = makeEquipInspectPacket();
    ok = expectBytes(encodedPacketBytes(equipInspect),
                     EquipInspectBytes{ 0xDD, 0x08, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11,
                                        0x88, 0x77, 0x66, 0x55, 0x02, 0xAA, 0xBB, 0xCC },
                     "EQUIP_INSPECT encoded packet") &&
         ok;
    ok = expectEqualInt(equipInspect.Kind, 2, "EQUIP_INSPECT Kind payload") && ok;

    const auto inspectMessage = makeInspectMessagePacket();
    auto       expectedInspectMessage = InspectMessageBytes{};
    expectedInspectMessage[0] = 0xDE;
    expectedInspectMessage[1] = 0x40;
    expectedInspectMessage[2] = 0xEF;
    expectedInspectMessage[3] = 0xBE;
    std::memcpy(expectedInspectMessage.data() + offsetof(GP_CLI_COMMAND_INSPECT_MESSAGE, sInspectMessage), "Bazaar", 6);
    expectedInspectMessage[offsetof(GP_CLI_COMMAND_INSPECT_MESSAGE, padding00)] = 0xEE;
    ok = expectBytes(encodedPacketBytes(inspectMessage), expectedInspectMessage, "INSPECT_MESSAGE encoded packet") && ok;
    ok = expectEqualInt(inspectMessage.sInspectMessage[0], 'B', "INSPECT_MESSAGE first message byte") && ok;
    ok = expectEqualInt(inspectMessage.padding00, 0xEE, "INSPECT_MESSAGE padding00 payload") && ok;
    return ok;
}

auto testDungeonInspectConstantsAndValidation() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_EQUIP_INSPECT_KIND::Check), 0, "EQUIP_INSPECT_KIND::Check") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_EQUIP_INSPECT_KIND::CheckName), 1, "EQUIP_INSPECT_KIND::CheckName") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_EQUIP_INSPECT_KIND::CheckParam), 2, "EQUIP_INSPECT_KIND::CheckParam") && ok;
    ok = expectValid(validateDungeonParamPure(), "DUNGEON_PARAM pure validation") && ok;
    ok = expectValid(validateEquipInspectPure(0), "EQUIP_INSPECT check validation") && ok;
    ok = expectValid(validateEquipInspectPure(1), "EQUIP_INSPECT checkname validation") && ok;
    ok = expectValid(validateEquipInspectPure(2), "EQUIP_INSPECT checkparam validation") && ok;
    ok = expectInvalidError(validateEquipInspectPure(3), "3 not a valid GP_CLI_COMMAND_EQUIP_INSPECT_KIND value.", "EQUIP_INSPECT invalid kind validation") && ok;
    ok = expectValid(validateInspectMessagePure(), "INSPECT_MESSAGE pure validation") && ok;
    return ok;
}

} // namespace

auto runC2SDungeonInspectPacketSelfTests() -> bool
{
    return testDungeonInspectLayoutsAndMetadata() &&
           testDungeonInspectEncodedBytesAndPayloads() &&
           testDungeonInspectConstantsAndValidation();
}
