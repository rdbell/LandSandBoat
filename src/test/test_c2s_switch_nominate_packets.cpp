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

#include "test_c2s_switch_nominate_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/packets/c2s/0x0a0_switch_proposal.h"
#include "map/packets/c2s/0x0a1_switch_vote.h"

namespace
{

constexpr auto proposalStrOffset  = 5U;
constexpr auto proposalStrMaxSize = 128U;
constexpr auto voteNameOffset     = 5U;
constexpr auto voteNameMaxSize    = 15U;

using ProposalShortBytes = std::array<std::uint8_t, 16>;
using ProposalFullBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SWITCH_PROPOSAL)>;
using VoteShortBytes     = std::array<std::uint8_t, 12>;
using VoteFullBytes      = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SWITCH_VOTE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s switch nominate packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s switch nominate packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s switch nominate packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s switch nominate packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s switch nominate packet self-test failed: " << label << " got";
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

auto validateSwitchProposalKind(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND>(kind);
    return validator;
}

auto validateSwitchVotePure(std::uint8_t index) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    (void)index;
    return validator;
}

auto makeProposalShortPacket() -> GP_CLI_COMMAND_SWITCH_PROPOSAL
{
    static constexpr auto text = std::string_view("Nominate");
    auto                  packet = GP_CLI_COMMAND_SWITCH_PROPOSAL{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SWITCH_PROPOSAL);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(proposalStrOffset + text.size())) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.Kind          = GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Shout;
    std::memcpy(packet.Str, text.data(), text.size());
    return packet;
}

auto makeProposalFullPacket() -> GP_CLI_COMMAND_SWITCH_PROPOSAL
{
    auto packet         = GP_CLI_COMMAND_SWITCH_PROPOSAL{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SWITCH_PROPOSAL);
    packet.header.size = roundUpToNearestFour(static_cast<uint32>(proposalStrOffset + proposalStrMaxSize)) / 4U;
    packet.header.sync = 0x1234;
    packet.Kind        = GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Party;
    for (std::size_t i = 0; i < proposalStrMaxSize; ++i)
    {
        packet.Str[i] = static_cast<std::uint8_t>('A' + (i % 26));
    }
    return packet;
}

auto expectedProposalFullPacket() -> ProposalFullBytes
{
    auto expected = ProposalFullBytes{};
    expected[0]   = 0xA0;
    expected[1]   = 0x44;
    expected[2]   = 0x34;
    expected[3]   = 0x12;
    expected[4]   = static_cast<std::uint8_t>(GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Party);
    for (std::size_t i = 0; i < proposalStrMaxSize; ++i)
    {
        expected[proposalStrOffset + i] = static_cast<std::uint8_t>('A' + (i % 26));
    }
    return expected;
}

auto makeVoteShortPacket() -> GP_CLI_COMMAND_SWITCH_VOTE
{
    static constexpr auto name = std::string_view("Alice");
    auto                  packet = GP_CLI_COMMAND_SWITCH_VOTE{};
    packet.header.id     = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SWITCH_VOTE);
    packet.header.size   = roundUpToNearestFour(static_cast<uint32>(voteNameOffset + name.size())) / 4U;
    packet.header.sync   = 0xBEEF;
    packet.Index         = 0x07;
    std::memcpy(packet.Name, name.data(), name.size());
    return packet;
}

auto makeVoteFullPacket() -> GP_CLI_COMMAND_SWITCH_VOTE
{
    auto packet         = GP_CLI_COMMAND_SWITCH_VOTE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SWITCH_VOTE);
    packet.header.size = roundUpToNearestFour(static_cast<uint32>(voteNameOffset + voteNameMaxSize)) / 4U;
    packet.header.sync = 0x1234;
    packet.Index       = 0x09;
    for (std::size_t i = 0; i < voteNameMaxSize; ++i)
    {
        packet.Name[i] = static_cast<std::uint8_t>('0' + (i % 10));
    }
    return packet;
}

auto expectedVoteFullPacket() -> VoteFullBytes
{
    auto expected = VoteFullBytes{};
    expected[0]   = 0xA1;
    expected[1]   = 0x0A;
    expected[2]   = 0x34;
    expected[3]   = 0x12;
    expected[4]   = 0x09;
    for (std::size_t i = 0; i < voteNameMaxSize; ++i)
    {
        expected[voteNameOffset + i] = static_cast<std::uint8_t>('0' + (i % 10));
    }
    return expected;
}

auto testSwitchProposalLayoutMetadataAndPayload() -> bool
{
    const auto shortPacket = makeProposalShortPacket();
    const auto fullPacket  = makeProposalFullPacket();
    bool       ok          = true;

    ok = expectEqualString(GP_CLI_COMMAND_SWITCH_PROPOSAL::name, "GP_CLI_COMMAND_SWITCH_PROPOSAL", "SWITCH_PROPOSAL name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SWITCH_PROPOSAL::packetId), 0x0A0, "SWITCH_PROPOSAL packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SWITCH_PROPOSAL), 134, "SWITCH_PROPOSAL sizeof") && ok;
    ok = expectEqualInt(roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_SWITCH_PROPOSAL))), 136, "SWITCH_PROPOSAL rounded sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SWITCH_PROPOSAL, Kind), 4, "SWITCH_PROPOSAL Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SWITCH_PROPOSAL, Str), 5, "SWITCH_PROPOSAL Str offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SWITCH_PROPOSAL::Str), 128, "SWITCH_PROPOSAL Str bytes") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_SWITCH_PROPOSAL, 16>(shortPacket),
                     ProposalShortBytes{ 0xA0, 0x08, 0xEF, 0xBE, 0x06, 0x4E, 0x6F, 0x6D,
                                         0x69, 0x6E, 0x61, 0x74, 0x65, 0x00, 0x00, 0x00 },
                     "SWITCH_PROPOSAL short encoded packet") &&
         ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_SWITCH_PROPOSAL, sizeof(GP_CLI_COMMAND_SWITCH_PROPOSAL)>(fullPacket),
                     expectedProposalFullPacket(),
                     "SWITCH_PROPOSAL full encoded packet") &&
         ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(shortPacket.Kind), 0x06, "SWITCH_PROPOSAL short Kind") && ok;
    ok = expectEqualInt(fullPacket.Str[127], static_cast<std::uint8_t>('A' + (127 % 26)), "SWITCH_PROPOSAL full final Str byte") && ok;
    return ok;
}

auto testSwitchVoteLayoutMetadataAndPayload() -> bool
{
    const auto shortPacket = makeVoteShortPacket();
    const auto fullPacket  = makeVoteFullPacket();
    bool       ok          = true;

    ok = expectEqualString(GP_CLI_COMMAND_SWITCH_VOTE::name, "GP_CLI_COMMAND_SWITCH_VOTE", "SWITCH_VOTE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SWITCH_VOTE::packetId), 0x0A1, "SWITCH_VOTE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SWITCH_VOTE), 20, "SWITCH_VOTE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SWITCH_VOTE, Index), 4, "SWITCH_VOTE Index offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SWITCH_VOTE, Name), 5, "SWITCH_VOTE Name offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SWITCH_VOTE::Name), 15, "SWITCH_VOTE Name bytes") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_SWITCH_VOTE, 12>(shortPacket),
                     VoteShortBytes{ 0xA1, 0x06, 0xEF, 0xBE, 0x07, 0x41, 0x6C, 0x69, 0x63, 0x65, 0x00, 0x00 },
                     "SWITCH_VOTE short encoded packet") &&
         ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_SWITCH_VOTE, sizeof(GP_CLI_COMMAND_SWITCH_VOTE)>(fullPacket),
                     expectedVoteFullPacket(),
                     "SWITCH_VOTE full encoded packet") &&
         ok;
    ok = expectEqualInt(shortPacket.Index, 0x07, "SWITCH_VOTE short Index") && ok;
    ok = expectEqualInt(fullPacket.Name[14], static_cast<std::uint8_t>('0' + (14 % 10)), "SWITCH_VOTE full final Name byte") && ok;
    return ok;
}

auto testSwitchProposalKindConstantsAndValidation() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Party), 1, "Kind::Party") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Linkshell1), 2, "Kind::Linkshell1") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Linkshell2), 3, "Kind::Linkshell2") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Say), 5, "Kind::Say") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Shout), 6, "Kind::Shout") && ok;
    ok      = expectValid(validateSwitchProposalKind(1), "SWITCH_PROPOSAL party validation") && ok;
    ok      = expectValid(validateSwitchProposalKind(2), "SWITCH_PROPOSAL linkshell1 validation") && ok;
    ok      = expectValid(validateSwitchProposalKind(3), "SWITCH_PROPOSAL linkshell2 validation") && ok;
    ok      = expectValid(validateSwitchProposalKind(5), "SWITCH_PROPOSAL say validation") && ok;
    ok      = expectValid(validateSwitchProposalKind(6), "SWITCH_PROPOSAL shout validation") && ok;
    ok      = expectInvalidError(validateSwitchProposalKind(0), "0 not a valid GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND value.", "SWITCH_PROPOSAL zero kind validation") && ok;
    ok      = expectInvalidError(validateSwitchProposalKind(4), "4 not a valid GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND value.", "SWITCH_PROPOSAL gap kind validation") && ok;
    ok      = expectValid(validateSwitchVotePure(0xFF), "SWITCH_VOTE pure validation") && ok;
    return ok;
}

} // namespace

auto runC2SSwitchNominatePacketSelfTests() -> bool
{
    return testSwitchProposalLayoutMetadataAndPayload() &&
           testSwitchVoteLayoutMetadataAndPayload() &&
           testSwitchProposalKindConstantsAndValidation();
}
