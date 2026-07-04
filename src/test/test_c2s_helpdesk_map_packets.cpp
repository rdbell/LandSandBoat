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

#include "test_c2s_helpdesk_map_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/packets/c2s/0x0d2_map_group.h"
#include "map/packets/c2s/0x0d3_faq_gmcall.h"
#include "map/packets/c2s/0x0d4_faq_gmparam.h"
#include "map/packets/c2s/0x0d5_ack_gmmsg.h"

namespace
{

constexpr auto faqGMCallDataOffset  = 8U;
constexpr auto faqGMCallDataMaxSize = 256U;

using MapGroupBytes       = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MAP_GROUP)>;
using FAQGMCallShortBytes = std::array<std::uint8_t, 16>;
using FAQGMCallFullBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_FAQ_GMCALL)>;
using FAQGMParamBytes     = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_FAQ_GMPARAM)>;
using AckGMMsgBytes       = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ACK_GMMSG)>;


auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s helpdesk/map packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s helpdesk/map packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s helpdesk/map packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s helpdesk/map packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s helpdesk/map packet self-test failed: " << label << " got";
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

template <typename Packet, std::size_t N>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, N>
{
    auto bytes = std::array<std::uint8_t, N>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto testBytes(std::size_t size, std::uint8_t start) -> std::array<std::uint8_t, faqGMCallDataMaxSize>
{
    auto bytes = std::array<std::uint8_t, faqGMCallDataMaxSize>{};
    for (std::size_t i = 0; i < size && i < bytes.size(); ++i)
    {
        bytes[i] = static_cast<std::uint8_t>(start + (i % 26));
    }
    return bytes;
}

auto validateMapGroupPure(std::uint32_t zoneNo) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    (void)zoneNo;
    return validator;
}

auto validateFAQGMCallPure(std::uint8_t type, std::uint8_t vers, std::uint16_t eos) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_FAQ_GMCALL_TYPE>(type)
        .mustEqual(vers, 0, "vers not equal to 0")
        .range("eos", eos, 0, 1);
    return validator;
}

auto validateFAQGMParamPure(std::uint16_t option) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(option, 0, "Option not 0");
    return validator;
}

auto validateAckGMMsgPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto makeMapGroupPacket() -> GP_CLI_COMMAND_MAP_GROUP
{
    auto packet         = GP_CLI_COMMAND_MAP_GROUP{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MAP_GROUP);
    packet.header.size = sizeof(GP_CLI_COMMAND_MAP_GROUP) / 4U;
    packet.header.sync = 0xBEEF;
    packet.ZoneNo      = 0x11223344;
    return packet;
}

auto makeFAQGMCallShortPacket() -> GP_CLI_COMMAND_FAQ_GMCALL
{
    static constexpr auto text = std::string_view("Hello");
    auto                  packet = GP_CLI_COMMAND_FAQ_GMCALL{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_FAQ_GMCALL);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(faqGMCallDataOffset + text.size())) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.type          = static_cast<std::uint16_t>(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMCall);
    packet.vers          = 0;
    packet.pktId         = 0x7A;
    packet.seq           = 0x12;
    packet.eos           = 1;
    packet.blkNum        = 0x09;
    std::memcpy(packet.Data, text.data(), text.size());
    return packet;
}

auto makeFAQGMCallFullPacket() -> GP_CLI_COMMAND_FAQ_GMCALL
{
    auto packet         = GP_CLI_COMMAND_FAQ_GMCALL{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_FAQ_GMCALL);
    packet.header.size = sizeof(GP_CLI_COMMAND_FAQ_GMCALL) / 4U;
    packet.header.sync = 0x1234;
    packet.type        = static_cast<std::uint16_t>(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::AddHistory);
    packet.vers        = 0;
    packet.pktId       = 0xFF;
    packet.seq         = 0x7F;
    packet.eos         = 1;
    packet.blkNum      = 0xFF;

    const auto data = testBytes(faqGMCallDataMaxSize, 'A');
    std::memcpy(packet.Data, data.data(), data.size());
    return packet;
}

auto makeFAQGMParamPacket() -> GP_CLI_COMMAND_FAQ_GMPARAM
{
    auto packet         = GP_CLI_COMMAND_FAQ_GMPARAM{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_FAQ_GMPARAM);
    packet.header.size = sizeof(GP_CLI_COMMAND_FAQ_GMPARAM) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Id          = 0x1234;
    packet.Option      = 0;
    return packet;
}

auto makeAckGMMsgPacket() -> GP_CLI_COMMAND_ACK_GMMSG
{
    auto packet         = GP_CLI_COMMAND_ACK_GMMSG{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ACK_GMMSG);
    packet.header.size = sizeof(GP_CLI_COMMAND_ACK_GMMSG) / 4U;
    packet.header.sync = 0xBEEF;
    packet.msgId       = 0x11223344;
    packet.seqId       = 0x5566;
    packet.seqNum      = 0x7788;
    return packet;
}

auto testHelpdeskMapLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_MAP_GROUP::name, "GP_CLI_COMMAND_MAP_GROUP", "MAP_GROUP name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MAP_GROUP::packetId), 0x0D2, "MAP_GROUP packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MAP_GROUP), 8, "MAP_GROUP sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MAP_GROUP, ZoneNo), 4, "MAP_GROUP ZoneNo offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_FAQ_GMCALL::name, "GP_CLI_COMMAND_FAQ_GMCALL", "FAQ_GMCALL name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FAQ_GMCALL::packetId), 0x0D3, "FAQ_GMCALL packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_FAQ_GMCALL), 264, "FAQ_GMCALL sizeof") && ok;
    ok = expectEqualInt(GP_CLI_COMMAND_FAQ_GMCALL::getMinSize(), faqGMCallDataOffset, "FAQ_GMCALL min size") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_FAQ_GMCALL))), 264, "FAQ_GMCALL rounded sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FAQ_GMCALL, Data), faqGMCallDataOffset, "FAQ_GMCALL Data offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_FAQ_GMCALL::Data), faqGMCallDataMaxSize, "FAQ_GMCALL Data bytes") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_FAQ_GMPARAM::name, "GP_CLI_COMMAND_FAQ_GMPARAM", "FAQ_GMPARAM name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FAQ_GMPARAM::packetId), 0x0D4, "FAQ_GMPARAM packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_FAQ_GMPARAM), 8, "FAQ_GMPARAM sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FAQ_GMPARAM, Id), 4, "FAQ_GMPARAM Id offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FAQ_GMPARAM, Option), 6, "FAQ_GMPARAM Option offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_ACK_GMMSG::name, "GP_CLI_COMMAND_ACK_GMMSG", "ACK_GMMSG name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_ACK_GMMSG::packetId), 0x0D5, "ACK_GMMSG packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ACK_GMMSG), 12, "ACK_GMMSG sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACK_GMMSG, msgId), 4, "ACK_GMMSG msgId offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACK_GMMSG, seqId), 8, "ACK_GMMSG seqId offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ACK_GMMSG, seqNum), 10, "ACK_GMMSG seqNum offset") && ok;
    return ok;
}

auto testHelpdeskMapEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    const auto mapGroup = makeMapGroupPacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_MAP_GROUP, sizeof(GP_CLI_COMMAND_MAP_GROUP)>(mapGroup),
                     MapGroupBytes{ 0xD2, 0x04, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11 },
                     "MAP_GROUP encoded packet") &&
         ok;
    ok = expectEqualInt(mapGroup.ZoneNo, 0x11223344, "MAP_GROUP ZoneNo payload") && ok;

    const auto faqCallShort = makeFAQGMCallShortPacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_FAQ_GMCALL, 16>(faqCallShort),
                     FAQGMCallShortBytes{ 0xD3, 0x08, 0xEF, 0xBE, 0x02, 0x7A, 0x92, 0x09,
                                          'H', 'e', 'l', 'l', 'o', 0x00, 0x00, 0x00 },
                     "FAQ_GMCALL short encoded packet") &&
         ok;
    ok = expectEqualInt(faqCallShort.type, 2, "FAQ_GMCALL type payload") && ok;
    ok = expectEqualInt(faqCallShort.vers, 0, "FAQ_GMCALL vers payload") && ok;
    ok = expectEqualInt(faqCallShort.pktId, 0x7A, "FAQ_GMCALL pktId payload") && ok;
    ok = expectEqualInt(faqCallShort.seq, 0x12, "FAQ_GMCALL seq payload") && ok;
    ok = expectEqualInt(faqCallShort.eos, 1, "FAQ_GMCALL eos payload") && ok;
    ok = expectEqualInt(faqCallShort.blkNum, 0x09, "FAQ_GMCALL blkNum payload") && ok;

    const auto faqCallFull = makeFAQGMCallFullPacket();
    auto       fullWant    = FAQGMCallFullBytes{};
    fullWant[0]            = 0xD3;
    fullWant[1]            = 0x84;
    fullWant[2]            = 0x34;
    fullWant[3]            = 0x12;
    fullWant[4]            = 0x01;
    fullWant[5]            = 0xFF;
    fullWant[6]            = 0xFF;
    fullWant[7]            = 0xFF;
    const auto fullData    = testBytes(faqGMCallDataMaxSize, 'A');
    std::memcpy(fullWant.data() + faqGMCallDataOffset, fullData.data(), fullData.size());
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_FAQ_GMCALL, sizeof(GP_CLI_COMMAND_FAQ_GMCALL)>(faqCallFull),
                     fullWant,
                     "FAQ_GMCALL full encoded packet") &&
         ok;

    const auto faqParam = makeFAQGMParamPacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_FAQ_GMPARAM, sizeof(GP_CLI_COMMAND_FAQ_GMPARAM)>(faqParam),
                     FAQGMParamBytes{ 0xD4, 0x04, 0xEF, 0xBE, 0x34, 0x12, 0x00, 0x00 },
                     "FAQ_GMPARAM encoded packet") &&
         ok;
    ok = expectEqualInt(faqParam.Id, 0x1234, "FAQ_GMPARAM Id payload") && ok;
    ok = expectEqualInt(faqParam.Option, 0, "FAQ_GMPARAM Option payload") && ok;

    const auto ack = makeAckGMMsgPacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_ACK_GMMSG, sizeof(GP_CLI_COMMAND_ACK_GMMSG)>(ack),
                     AckGMMsgBytes{ 0xD5, 0x06, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11, 0x66, 0x55, 0x88, 0x77 },
                     "ACK_GMMSG encoded packet") &&
         ok;
    ok = expectEqualInt(ack.msgId, 0x11223344, "ACK_GMMSG msgId payload") && ok;
    ok = expectEqualInt(ack.seqId, 0x5566, "ACK_GMMSG seqId payload") && ok;
    ok = expectEqualInt(ack.seqNum, 0x7788, "ACK_GMMSG seqNum payload") && ok;
    return ok;
}

auto testHelpdeskMapConstantsAndValidation() -> bool
{
    bool ok = true;

    ok = expectValid(validateMapGroupPure(0xFFFF), "MAP_GROUP pure validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::AddHistory), 1, "FAQ_GMCALL_TYPE::AddHistory") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMCall), 2, "FAQ_GMCALL_TYPE::GMCall") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMNotice), 3, "FAQ_GMCALL_TYPE::GMNotice") && ok;
    ok = expectValid(validateFAQGMCallPure(1, 0, 0), "FAQ_GMCALL add history validation") && ok;
    ok = expectValid(validateFAQGMCallPure(2, 0, 1), "FAQ_GMCALL gm call validation") && ok;
    ok = expectValid(validateFAQGMCallPure(3, 0, 0), "FAQ_GMCALL gm notice validation") && ok;
    ok = expectInvalidError(validateFAQGMCallPure(0, 0, 0), "0 not a valid GP_CLI_COMMAND_FAQ_GMCALL_TYPE value.", "FAQ_GMCALL invalid type validation") && ok;
    ok = expectInvalidError(validateFAQGMCallPure(2, 1, 0), "vers not equal to 0", "FAQ_GMCALL invalid vers validation") && ok;
    ok = expectInvalidError(validateFAQGMCallPure(2, 0, 2), "eos out of range: 2 not in [0, 1]", "FAQ_GMCALL invalid eos validation") && ok;
    ok = expectInvalidError(validateFAQGMCallPure(0, 1, 2), "0 not a valid GP_CLI_COMMAND_FAQ_GMCALL_TYPE value.", "FAQ_GMCALL validation order") && ok;

    ok = expectValid(validateFAQGMParamPure(0), "FAQ_GMPARAM pure validation") && ok;
    ok = expectInvalidError(validateFAQGMParamPure(1), "Option not 0", "FAQ_GMPARAM invalid Option validation") && ok;
    ok = expectValid(validateAckGMMsgPure(), "ACK_GMMSG pure validation") && ok;
    return ok;
}

} // namespace

auto runC2SHelpdeskMapPacketSelfTests() -> bool
{
    return testHelpdeskMapLayoutsAndMetadata() &&
           testHelpdeskMapEncodedBytesAndPayloads() &&
           testHelpdeskMapConstantsAndValidation();
}
