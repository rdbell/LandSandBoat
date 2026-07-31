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

#include "test_c2s_chat_assist_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/packets/c2s/0x0b5_chat_std.h"
#include "map/packets/c2s/chat_std_capacity.h"
#include "map/packets/c2s/0x0b6_chat_name.h"
#include "map/packets/c2s/0x0b7_assist_channel.h"

namespace
{

constexpr auto chatStdStrOffset        = 6U;
constexpr auto chatStdStrMaxSize       = 128U;
constexpr auto chatNameSNameOffset     = 6U;
constexpr auto chatNameMesOffset       = 21U;
constexpr auto chatNameMesMaxSize      = 128U;
constexpr auto assistChannelSNameOffset = 6U;
constexpr auto assistChannelMesOffset   = 21U;

using ChatStdShortBytes       = std::array<std::uint8_t, 12>;
using ChatStdFullBytes        = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CHAT_STD)>;
using ChatNameShortBytes      = std::array<std::uint8_t, 28>;
using ChatNameFullBytes       = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CHAT_NAME)>;
using AssistChannelFrameBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ASSIST_CHANNEL)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s chat/assist packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s chat/assist packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s chat/assist packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s chat/assist packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s chat/assist packet self-test failed: " << label << " got";
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

auto validateChatStdKind(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_CHAT_STD_KIND>(kind);
    return validator;
}

auto validateChatNamePure(std::uint8_t unknown04, std::uint8_t unknown05) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(unknown04, 3, "unknown04 not 3")
        .mustEqual(unknown05, 0, "unknown05 not 0");
    return validator;
}

auto validateAssistChannelKind(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_ASSIST_CHANNEL_KIND>(kind);
    return validator;
}

auto makeChatStdShortPacket() -> GP_CLI_COMMAND_CHAT_STD
{
    static constexpr auto text = std::string_view("Hello");
    auto                  packet = GP_CLI_COMMAND_CHAT_STD{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CHAT_STD);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(chatStdStrOffset + text.size())) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.Kind          = static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Yell);
    packet.unknown00     = 0x44;
    std::memcpy(packet.Str, text.data(), text.size());
    return packet;
}

auto makeChatStdFullPacket() -> GP_CLI_COMMAND_CHAT_STD
{
    auto packet         = GP_CLI_COMMAND_CHAT_STD{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CHAT_STD);
    packet.header.size = roundUpToNearestFour(static_cast<uint32>(chatStdStrOffset + chatStdStrMaxSize)) / 4U;
    packet.header.sync = 0x1234;
    packet.Kind        = static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Say);
    for (std::size_t i = 0; i < chatStdStrMaxSize; ++i)
    {
        packet.Str[i] = static_cast<std::uint8_t>('A' + (i % 26));
    }
    return packet;
}

auto expectedChatStdFullPacket() -> ChatStdFullBytes
{
    auto expected = ChatStdFullBytes{};
    expected[0]   = 0xB5;
    expected[1]   = 0x44;
    expected[2]   = 0x34;
    expected[3]   = 0x12;
    expected[4]   = static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Say);
    for (std::size_t i = 0; i < chatStdStrMaxSize; ++i)
    {
        expected[chatStdStrOffset + i] = static_cast<std::uint8_t>('A' + (i % 26));
    }
    return expected;
}

auto makeChatNameShortPacket() -> GP_CLI_COMMAND_CHAT_NAME
{
    static constexpr auto name = std::string_view("Bob");
    static constexpr auto text = std::string_view("Tell");
    auto                  packet = GP_CLI_COMMAND_CHAT_NAME{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CHAT_NAME);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(chatNameMesOffset + text.size())) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.unknown04     = 3;
    packet.unknown05     = 0;
    std::memcpy(packet.sName, name.data(), name.size());
    std::memcpy(packet.Mes, text.data(), text.size());
    return packet;
}

auto makeChatNameFullPacket() -> GP_CLI_COMMAND_CHAT_NAME
{
    static constexpr auto name = std::string_view("Charlie");
    auto                  packet = GP_CLI_COMMAND_CHAT_NAME{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CHAT_NAME);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(chatNameMesOffset + chatNameMesMaxSize)) / 4U;
    packet.header.sync   = 0x1234;
    packet.unknown04     = 3;
    packet.unknown05     = 0;
    std::memcpy(packet.sName, name.data(), name.size());
    for (std::size_t i = 0; i < chatNameMesMaxSize; ++i)
    {
        packet.Mes[i] = static_cast<std::uint8_t>('0' + (i % 10));
    }
    return packet;
}

auto expectedChatNameFullPacket() -> ChatNameFullBytes
{
    static constexpr auto name = std::string_view("Charlie");
    auto                  expected = ChatNameFullBytes{};
    expected[0]           = 0xB6;
    expected[1]           = 0x4C;
    expected[2]           = 0x34;
    expected[3]           = 0x12;
    expected[4]           = 0x03;
    std::memcpy(expected.data() + chatNameSNameOffset, name.data(), name.size());
    for (std::size_t i = 0; i < chatNameMesMaxSize; ++i)
    {
        expected[chatNameMesOffset + i] = static_cast<std::uint8_t>('0' + (i % 10));
    }
    return expected;
}

auto makeAssistChannelPacket() -> GP_CLI_COMMAND_ASSIST_CHANNEL
{
    static constexpr auto name = std::string_view("Alice");
    auto                  packet = GP_CLI_COMMAND_ASSIST_CHANNEL{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ASSIST_CHANNEL);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_ASSIST_CHANNEL))) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.Kind          = static_cast<std::uint8_t>(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp);
    packet.unknown00     = 0x01;
    std::memcpy(packet.sName, name.data(), name.size());
    packet.Mes[0] = ' ';
    return packet;
}

auto expectedAssistChannelPacket() -> AssistChannelFrameBytes
{
    static constexpr auto name = std::string_view("Alice");
    auto                  expected = AssistChannelFrameBytes{};
    expected[0]           = 0xB7;
    expected[1]           = 0x0C;
    expected[2]           = 0xEF;
    expected[3]           = 0xBE;
    expected[4]           = static_cast<std::uint8_t>(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp);
    expected[5]           = 0x01;
    std::memcpy(expected.data() + assistChannelSNameOffset, name.data(), name.size());
    expected[assistChannelMesOffset] = ' ';
    return expected;
}

auto testChatStdLayoutMetadataAndPayload() -> bool
{
    const auto shortPacket = makeChatStdShortPacket();
    const auto fullPacket  = makeChatStdFullPacket();
    bool       ok          = true;

    ok = expectEqualString(GP_CLI_COMMAND_CHAT_STD::name, "GP_CLI_COMMAND_CHAT_STD", "CHAT_STD name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CHAT_STD::packetId), 0x0B5, "CHAT_STD packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHAT_STD), 134, "CHAT_STD sizeof") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_CHAT_STD))), 136, "CHAT_STD rounded sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHAT_STD, Kind), 4, "CHAT_STD Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHAT_STD, unknown00), 5, "CHAT_STD unknown00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHAT_STD, Str), 6, "CHAT_STD Str offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHAT_STD::Str), 128, "CHAT_STD Str bytes") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_CHAT_STD, 12>(shortPacket),
                     ChatStdShortBytes{ 0xB5, 0x06, 0xEF, 0xBE, 0x1A, 0x44, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x00 },
                     "CHAT_STD short encoded packet") &&
         ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_CHAT_STD, sizeof(GP_CLI_COMMAND_CHAT_STD)>(fullPacket),
                     expectedChatStdFullPacket(),
                     "CHAT_STD full encoded packet") &&
         ok;
    ok = expectEqualInt(shortPacket.Kind, 0x1A, "CHAT_STD short Kind") && ok;
    ok = expectEqualInt(fullPacket.Str[127], static_cast<std::uint8_t>('A' + (127 % 26)), "CHAT_STD full final Str byte") && ok;
    ok = expectEqualInt(chatstdhelpers::BoundedMessageLength(0), chatstdhelpers::MaxMessageLength, "CHAT_STD wrapped zero length") && ok;
    ok = expectEqualInt(chatstdhelpers::BoundedMessageLength(-1), chatstdhelpers::MaxMessageLength, "CHAT_STD wrapped negative length") && ok;
    ok = expectEqualInt(chatstdhelpers::BoundedMessageLength(chatstdhelpers::MessageOffset), 0, "CHAT_STD message offset") && ok;
    ok = expectEqualInt(chatstdhelpers::BoundedMessageLength(chatstdhelpers::MessageOffset + 10), 10, "CHAT_STD bounded length") && ok;
    ok = expectEqualInt(chatstdhelpers::BoundedMessageLength(chatstdhelpers::MessageOffset + 999), chatstdhelpers::MaxMessageLength, "CHAT_STD length cap") && ok;
    return ok;
}

auto testChatNameLayoutMetadataAndPayload() -> bool
{
    const auto shortPacket = makeChatNameShortPacket();
    const auto fullPacket  = makeChatNameFullPacket();
    bool       ok          = true;

    ok = expectEqualString(GP_CLI_COMMAND_CHAT_NAME::name, "GP_CLI_COMMAND_CHAT_NAME", "CHAT_NAME name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CHAT_NAME::packetId), 0x0B6, "CHAT_NAME packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHAT_NAME), 150, "CHAT_NAME sizeof") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_CHAT_NAME))), 152, "CHAT_NAME rounded sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHAT_NAME, unknown04), 4, "CHAT_NAME unknown04 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHAT_NAME, unknown05), 5, "CHAT_NAME unknown05 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHAT_NAME, sName), 6, "CHAT_NAME sName offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHAT_NAME, Mes), 21, "CHAT_NAME Mes offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHAT_NAME::sName), 15, "CHAT_NAME sName bytes") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHAT_NAME::Mes), 128, "CHAT_NAME Mes bytes") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_CHAT_NAME, 28>(shortPacket),
                     ChatNameShortBytes{ 0xB6, 0x0E, 0xEF, 0xBE, 0x03, 0x00, 0x42, 0x6F,
                                         0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x65, 0x6C,
                                         0x6C, 0x00, 0x00, 0x00 },
                     "CHAT_NAME short encoded packet") &&
         ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_CHAT_NAME, sizeof(GP_CLI_COMMAND_CHAT_NAME)>(fullPacket),
                     expectedChatNameFullPacket(),
                     "CHAT_NAME full encoded packet") &&
         ok;
    ok = expectEqualInt(fullPacket.Mes[127], static_cast<std::uint8_t>('0' + (127 % 10)), "CHAT_NAME full final Mes byte") && ok;
    return ok;
}

auto testAssistChannelLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeAssistChannelPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_ASSIST_CHANNEL::name, "GP_CLI_COMMAND_ASSIST_CHANNEL", "ASSIST_CHANNEL name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_ASSIST_CHANNEL::packetId), 0x0B7, "ASSIST_CHANNEL packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ASSIST_CHANNEL), 22, "ASSIST_CHANNEL sizeof") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_ASSIST_CHANNEL))), 24, "ASSIST_CHANNEL rounded sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ASSIST_CHANNEL, Kind), 4, "ASSIST_CHANNEL Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ASSIST_CHANNEL, unknown00), 5, "ASSIST_CHANNEL unknown00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ASSIST_CHANNEL, sName), 6, "ASSIST_CHANNEL sName offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ASSIST_CHANNEL, Mes), 21, "ASSIST_CHANNEL Mes offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ASSIST_CHANNEL::sName), 15, "ASSIST_CHANNEL sName bytes") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ASSIST_CHANNEL::Mes), 1, "ASSIST_CHANNEL Mes bytes") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_ASSIST_CHANNEL, sizeof(GP_CLI_COMMAND_ASSIST_CHANNEL)>(packet),
                     expectedAssistChannelPacket(),
                     "ASSIST_CHANNEL encoded packet") &&
         ok;
    ok = expectEqualInt(packet.Mes[0], static_cast<std::uint8_t>(' '), "ASSIST_CHANNEL Mes space") && ok;
    return ok;
}

auto testChatAssistConstantsAndValidation() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Say), 0x00, "CHAT_STD Kind::Say") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Shout), 0x01, "CHAT_STD Kind::Shout") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Party), 0x04, "CHAT_STD Kind::Party") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Linkshell1), 0x05, "CHAT_STD Kind::Linkshell1") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Emote), 0x08, "CHAT_STD Kind::Emote") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::LinkshellPvp), 0x18, "CHAT_STD Kind::LinkshellPvp") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Yell), 0x1A, "CHAT_STD Kind::Yell") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Linkshell2), 0x1B, "CHAT_STD Kind::Linkshell2") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::Unity), 0x21, "CHAT_STD Kind::Unity") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::AssistJ), 0x22, "CHAT_STD Kind::AssistJ") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CHAT_STD_KIND::AssistE), 0x23, "CHAT_STD Kind::AssistE") && ok;
    ok      = expectValid(validateChatStdKind(0x00), "CHAT_STD say validation") && ok;
    ok      = expectValid(validateChatStdKind(0x23), "CHAT_STD assist e validation") && ok;
    ok      = expectInvalidError(validateChatStdKind(0x02), "2 not a valid GP_CLI_COMMAND_CHAT_STD_KIND value.", "CHAT_STD invalid kind validation") && ok;

    ok = expectValid(validateChatNamePure(3, 0), "CHAT_NAME pure validation") && ok;
    ok = expectInvalidError(validateChatNamePure(2, 0), "unknown04 not 3", "CHAT_NAME unknown04 validation") && ok;
    ok = expectInvalidError(validateChatNamePure(3, 1), "unknown05 not 0", "CHAT_NAME unknown05 validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp), 0x24, "ASSIST_CHANNEL Kind::GiveThumbsUp") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning), 0x25, "ASSIST_CHANNEL Kind::IssueWarning") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::AddToMuteList), 0x26, "ASSIST_CHANNEL Kind::AddToMuteList") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::RemoveFromMuteList), 0x27, "ASSIST_CHANNEL Kind::RemoveFromMuteList") && ok;
    ok = expectValid(validateAssistChannelKind(0x24), "ASSIST_CHANNEL thumbs up validation") && ok;
    ok = expectValid(validateAssistChannelKind(0x27), "ASSIST_CHANNEL remove mute validation") && ok;
    ok = expectInvalidError(validateAssistChannelKind(0x28), "40 not a valid GP_CLI_COMMAND_ASSIST_CHANNEL_KIND value.", "ASSIST_CHANNEL invalid kind validation") && ok;
    return ok;
}

} // namespace

auto runC2SChatAssistPacketSelfTests() -> bool
{
    return testChatStdLayoutMetadataAndPayload() &&
           testChatNameLayoutMetadataAndPayload() &&
           testAssistChannelLayoutMetadataAndPayload() &&
           testChatAssistConstantsAndValidation();
}
