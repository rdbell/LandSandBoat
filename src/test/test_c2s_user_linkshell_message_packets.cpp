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

#include "test_c2s_user_linkshell_message_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/linkshell.h"
#include "map/packets/c2s/0x0e0_set_usermsg.h"
#include "map/packets/c2s/0x0e1_get_lsmsg.h"
#include "map/packets/c2s/0x0e2_set_lsmsg.h"
#include "map/packets/c2s/0x0e4_get_lspriv.h"

namespace
{

using SetUserMsgBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SET_USERMSG)>;
using GetLSMsgBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GET_LSMSG)>;
using SetLSMsgBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SET_LSMSG)>;
using GetLSPrivBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GET_LSPRIV)>;

constexpr auto linkshellMessageFlags0Offset  = 4U;
constexpr auto linkshellMessageFlags1Offset  = 5U;
constexpr auto linkshellMessageCategoryOffset = 6U;
constexpr auto linkshellMessageItemIndexOffset = 7U;
constexpr auto linkshellMessagePadding00Offset = 8U;
constexpr auto linkshellMessageSeqIdOffset   = 10U;
constexpr auto linkshellMessageUniqNoOffset  = 12U;
constexpr auto linkshellMessageSMessageOffset = 16U;
constexpr auto linkshellMessageSMessageBytes = 128U;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s user/linkshell message packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s user/linkshell message packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s user/linkshell message packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s user/linkshell message packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s user/linkshell message packet self-test failed: " << label << " got";
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

auto validateSetUserMsgPure(std::uint32_t msgType) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_SET_USERMSG_MSGTYPE>(msgType);
    return validator;
}

auto validateGetLSMsgPure(std::uint8_t linkshellId) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<LinkshellSlot>(linkshellId);
    return validator;
}

auto validateSetLSMsgPure(std::uint8_t writeLevel) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL>(writeLevel);
    return validator;
}

auto validateGetLSPrivPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto makeSetUserMsgPacket() -> GP_CLI_COMMAND_SET_USERMSG
{
    auto packet         = GP_CLI_COMMAND_SET_USERMSG{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SET_USERMSG);
    packet.header.size = sizeof(GP_CLI_COMMAND_SET_USERMSG) / 4U;
    packet.header.sync = 0xBEEF;
    std::memcpy(packet.sMessage, "Search", 6);
    packet.installTime = 0x11223344;
    packet.platform[0] = 'W';
    packet.platform[1] = 'i';
    packet.platform[2] = 'n';
    packet.platform[3] = 0;
    packet.srvExCode.has_final_fantasy_xi   = 1;
    packet.srvExCode.has_rise_of_the_zilart = 1;
    packet.srvExCode.has_seekers_of_adoulin = 1;
    packet.cliExCode.has_final_fantasy_xi       = 1;
    packet.cliExCode.has_chains_of_promathia    = 1;
    packet.cliExCode.has_wings_of_the_goddess   = 1;
    packet.msgType = static_cast<std::uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::LinkshellRecruiting);
    return packet;
}

template <typename Packet>
auto fillLinkshellMessageFields(Packet& packet) -> void
{
    packet.unknown00    = 0x0A;
    packet.unknown01    = 1;
    packet.unknown02    = 0;
    packet.unknown03    = 1;
    packet.unknown04    = 1;
    packet.readLevel    = 2;
    packet.writeLevel   = static_cast<std::uint8_t>(GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Pearlsack);
    packet.pubEditLevel = 3;
    packet.LinkshellId  = static_cast<std::uint8_t>(LinkshellSlot::LS2);
    packet.Category     = 0x12;
    packet.ItemIndex    = 0x34;
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.seqId        = 0x5678;
    packet.uniqNo       = 0x11223344;
    std::memcpy(packet.sMessage, "Linkshell", 9);
}

auto makeGetLSMsgPacket() -> GP_CLI_COMMAND_GET_LSMSG
{
    auto packet         = GP_CLI_COMMAND_GET_LSMSG{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GET_LSMSG);
    packet.header.size = sizeof(GP_CLI_COMMAND_GET_LSMSG) / 4U;
    packet.header.sync = 0xBEEF;
    fillLinkshellMessageFields(packet);
    return packet;
}

auto makeSetLSMsgPacket() -> GP_CLI_COMMAND_SET_LSMSG
{
    auto packet         = GP_CLI_COMMAND_SET_LSMSG{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SET_LSMSG);
    packet.header.size = sizeof(GP_CLI_COMMAND_SET_LSMSG) / 4U;
    packet.header.sync = 0xBEEF;
    fillLinkshellMessageFields(packet);
    return packet;
}

auto makeGetLSPrivPacket() -> GP_CLI_COMMAND_GET_LSPRIV
{
    auto packet         = GP_CLI_COMMAND_GET_LSPRIV{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GET_LSPRIV);
    packet.header.size = sizeof(GP_CLI_COMMAND_GET_LSPRIV) / 4U;
    packet.header.sync = 0xBEEF;
    fillLinkshellMessageFields(packet);
    return packet;
}

auto expectedSetUserMsgBytes() -> SetUserMsgBytes
{
    auto expected = SetUserMsgBytes{};
    expected[0] = 0xE0;
    expected[1] = 0x4C;
    expected[2] = 0xEF;
    expected[3] = 0xBE;
    std::memcpy(expected.data() + offsetof(GP_CLI_COMMAND_SET_USERMSG, sMessage), "Search", 6);
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, installTime) + 0] = 0x44;
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, installTime) + 1] = 0x33;
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, installTime) + 2] = 0x22;
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, installTime) + 3] = 0x11;
    std::memcpy(expected.data() + offsetof(GP_CLI_COMMAND_SET_USERMSG, platform), "Win", 3);
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, srvExCode) + 0] = 0x03;
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, srvExCode) + 1] = 0x08;
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, cliExCode) + 0] = 0x15;
    expected[offsetof(GP_CLI_COMMAND_SET_USERMSG, msgType) + 0] = 0x52;
    return expected;
}

template <typename Bytes>
auto expectedLinkshellMessageBytes(std::uint8_t packetId) -> Bytes
{
    auto expected = Bytes{};
    expected[0] = packetId;
    expected[1] = 0x48;
    expected[2] = 0xEF;
    expected[3] = 0xBE;
    expected[linkshellMessageFlags0Offset] = 0xDA;
    expected[linkshellMessageFlags1Offset] = 0x76;
    expected[linkshellMessageCategoryOffset] = 0x12;
    expected[linkshellMessageItemIndexOffset] = 0x34;
    expected[linkshellMessagePadding00Offset] = 0xAA;
    expected[linkshellMessagePadding00Offset + 1] = 0xBB;
    expected[linkshellMessageSeqIdOffset] = 0x78;
    expected[linkshellMessageSeqIdOffset + 1] = 0x56;
    expected[linkshellMessageUniqNoOffset] = 0x44;
    expected[linkshellMessageUniqNoOffset + 1] = 0x33;
    expected[linkshellMessageUniqNoOffset + 2] = 0x22;
    expected[linkshellMessageUniqNoOffset + 3] = 0x11;
    std::memcpy(expected.data() + linkshellMessageSMessageOffset, "Linkshell", 9);
    return expected;
}

auto testUserLinkshellMessageLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_SET_USERMSG::name, "GP_CLI_COMMAND_SET_USERMSG", "SET_USERMSG name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SET_USERMSG::packetId), 0x0E0, "SET_USERMSG packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SET_USERMSG), 152, "SET_USERMSG sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SET_USERMSG, sMessage), 4, "SET_USERMSG sMessage offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SET_USERMSG::sMessage), 128, "SET_USERMSG sMessage bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SET_USERMSG, installTime), 132, "SET_USERMSG installTime offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SET_USERMSG, platform), 136, "SET_USERMSG platform offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SET_USERMSG::platform), 4, "SET_USERMSG platform bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SET_USERMSG, srvExCode), 140, "SET_USERMSG srvExCode offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SET_USERMSG::srvExCode), 4, "SET_USERMSG srvExCode bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SET_USERMSG, cliExCode), 144, "SET_USERMSG cliExCode offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SET_USERMSG, msgType), 148, "SET_USERMSG msgType offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GET_LSMSG::name, "GP_CLI_COMMAND_GET_LSMSG", "GET_LSMSG name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GET_LSMSG::packetId), 0x0E1, "GET_LSMSG packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GET_LSMSG), 144, "GET_LSMSG sizeof") && ok;
    ok = expectEqualString(GP_CLI_COMMAND_SET_LSMSG::name, "GP_CLI_COMMAND_SET_LSMSG", "SET_LSMSG name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SET_LSMSG::packetId), 0x0E2, "SET_LSMSG packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SET_LSMSG), 144, "SET_LSMSG sizeof") && ok;
    ok = expectEqualString(GP_CLI_COMMAND_GET_LSPRIV::name, "GP_CLI_COMMAND_GET_LSPRIV", "GET_LSPRIV name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GET_LSPRIV::packetId), 0x0E4, "GET_LSPRIV packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GET_LSPRIV), 144, "GET_LSPRIV sizeof") && ok;

    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GET_LSMSG, Category), linkshellMessageCategoryOffset, "GET_LSMSG Category offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GET_LSMSG, ItemIndex), linkshellMessageItemIndexOffset, "GET_LSMSG ItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GET_LSMSG, padding00), linkshellMessagePadding00Offset, "GET_LSMSG padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GET_LSMSG::padding00), 2, "GET_LSMSG padding00 bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GET_LSMSG, seqId), linkshellMessageSeqIdOffset, "GET_LSMSG seqId offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GET_LSMSG, uniqNo), linkshellMessageUniqNoOffset, "GET_LSMSG uniqNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GET_LSMSG, sMessage), linkshellMessageSMessageOffset, "GET_LSMSG sMessage offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GET_LSMSG::sMessage), linkshellMessageSMessageBytes, "GET_LSMSG sMessage bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SET_LSMSG, Category), linkshellMessageCategoryOffset, "SET_LSMSG Category offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GET_LSPRIV, Category), linkshellMessageCategoryOffset, "GET_LSPRIV Category offset") && ok;
    return ok;
}

auto testUserLinkshellMessageEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    const auto setUserMsg = makeSetUserMsgPacket();
    ok = expectBytes(encodedPacketBytes(setUserMsg), expectedSetUserMsgBytes(), "SET_USERMSG encoded packet") && ok;
    ok = expectEqualInt(setUserMsg.srvExCode.has_seekers_of_adoulin, 1, "SET_USERMSG srvExCode Adoulin bit") && ok;
    ok = expectEqualInt(setUserMsg.cliExCode.has_chains_of_promathia, 1, "SET_USERMSG cliExCode Promathia bit") && ok;
    ok = expectEqualInt(setUserMsg.msgType, 0x52, "SET_USERMSG msgType payload") && ok;

    const auto getLSMsg = makeGetLSMsgPacket();
    ok = expectBytes(encodedPacketBytes(getLSMsg), expectedLinkshellMessageBytes<GetLSMsgBytes>(0xE1), "GET_LSMSG encoded packet") && ok;
    ok = expectEqualInt(getLSMsg.unknown00, 0x0A, "GET_LSMSG unknown00 payload") && ok;
    ok = expectEqualInt(getLSMsg.LinkshellId, 1, "GET_LSMSG LinkshellId payload") && ok;

    const auto setLSMsg = makeSetLSMsgPacket();
    ok = expectBytes(encodedPacketBytes(setLSMsg), expectedLinkshellMessageBytes<SetLSMsgBytes>(0xE2), "SET_LSMSG encoded packet") && ok;
    ok = expectEqualInt(setLSMsg.writeLevel, 1, "SET_LSMSG writeLevel payload") && ok;

    const auto getLSPriv = makeGetLSPrivPacket();
    ok = expectBytes(encodedPacketBytes(getLSPriv), expectedLinkshellMessageBytes<GetLSPrivBytes>(0xE4), "GET_LSPRIV encoded packet") && ok;
    ok = expectEqualInt(getLSPriv.seqId, 0x5678, "GET_LSPRIV seqId payload") && ok;
    return ok;
}

auto testUserLinkshellMessageConstantsAndValidation() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::Default), 0, "SET_USERMSG_MSGTYPE::Default") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::EXPPartySeekParty), 0x11, "SET_USERMSG_MSGTYPE::EXPPartySeekParty") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::LinkshellRecruiting), 0x52, "SET_USERMSG_MSGTYPE::LinkshellRecruiting") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::Others), 0x73, "SET_USERMSG_MSGTYPE::Others") && ok;
    ok = expectValid(validateSetUserMsgPure(0), "SET_USERMSG default validation") && ok;
    ok = expectValid(validateSetUserMsgPure(0x11), "SET_USERMSG exp party validation") && ok;
    ok = expectValid(validateSetUserMsgPure(0x52), "SET_USERMSG linkshell validation") && ok;
    ok = expectInvalidError(validateSetUserMsgPure(0x72), "114 not a valid GP_CLI_COMMAND_SET_USERMSG_MSGTYPE value.", "SET_USERMSG invalid msgType validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(LinkshellSlot::LS1), 0, "LinkshellSlot::LS1") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(LinkshellSlot::LS2), 1, "LinkshellSlot::LS2") && ok;
    ok = expectValid(validateGetLSMsgPure(0), "GET_LSMSG LS1 validation") && ok;
    ok = expectValid(validateGetLSMsgPure(1), "GET_LSMSG LS2 validation") && ok;
    ok = expectInvalidError(validateGetLSMsgPure(2), "2 not a valid LinkshellSlot value.", "GET_LSMSG invalid LinkshellId validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell), 0, "SET_LSMSG_WRITELEVEL::Linkshell") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Pearlsack), 1, "SET_LSMSG_WRITELEVEL::Pearlsack") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkpearl), 2, "SET_LSMSG_WRITELEVEL::Linkpearl") && ok;
    ok = expectValid(validateSetLSMsgPure(0), "SET_LSMSG linkshell validation") && ok;
    ok = expectValid(validateSetLSMsgPure(1), "SET_LSMSG pearlsack validation") && ok;
    ok = expectValid(validateSetLSMsgPure(2), "SET_LSMSG linkpearl validation") && ok;
    ok = expectInvalidError(validateSetLSMsgPure(3), "3 not a valid GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL value.", "SET_LSMSG invalid writeLevel validation") && ok;
    ok = expectValid(validateGetLSPrivPure(), "GET_LSPRIV pure validation") && ok;
    return ok;
}

} // namespace

auto runC2SUserLinkshellMessagePacketSelfTests() -> bool
{
    return testUserLinkshellMessageLayoutsAndMetadata() &&
           testUserLinkshellMessageEncodedBytesAndPayloads() &&
           testUserLinkshellMessageConstantsAndValidation();
}
